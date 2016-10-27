/* Handle errors.
   Copyright (C) 2000-2015 Free Software Foundation, Inc.
   Contributed by Andy Vaught & Niels Kristian Bech Jensen

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

/* Handle the inevitable errors.  A major catch here is that things
   flagged as errors in one match subroutine can conceivably be legal
   elsewhere.  This means that error messages are recorded and saved
   for possible use later.  If a line does not match a legal
   construction, then the saved error message is reported.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "flags.h"
#include "gfortran.h"

#include "diagnostic.h"
#include "diagnostic-color.h"
#include "tree-diagnostic.h" /* tree_diagnostics_defaults */

#include <new> /* For placement-new */

static int suppress_errors = 0;

static bool warnings_not_errors = false;

static int terminal_width, errors, warnings;

static gfc_error_buf error_buffer, warning_buffer, *cur_error_buffer;

/* True if the error/warnings should be buffered.  */
static bool buffered_p;
/* These are always buffered buffers (.flush_p == false) to be used by
   the pretty-printer.  */
static output_buffer *pp_error_buffer, *pp_warning_buffer;
static int warningcount_buffered, werrorcount_buffered;

/* Return true if there output_buffer is empty.  */

static bool
gfc_output_buffer_empty_p (const output_buffer * buf)
{
  return output_buffer_last_position_in_text (buf) == NULL;
}

/* Go one level deeper suppressing errors.  */

void
gfc_push_suppress_errors (void)
{
  gcc_assert (suppress_errors >= 0);
  ++suppress_errors;
}

static void
gfc_error (const char *gmsgid, va_list ap)  ATTRIBUTE_GCC_GFC(1,0);

static bool
gfc_warning (int opt, const char *gmsgid, va_list ap) ATTRIBUTE_GCC_GFC(2,0);


/* Leave one level of error suppressing.  */

void
gfc_pop_suppress_errors (void)
{
  gcc_assert (suppress_errors > 0);
  --suppress_errors;
}


/* Determine terminal width (for trimming source lines in output).  */

static int
gfc_get_terminal_width (void)
{
  return isatty (STDERR_FILENO) ? get_terminal_width () : INT_MAX;
}


/* Per-file error initialization.  */

void
gfc_error_init_1 (void)
{
  terminal_width = gfc_get_terminal_width ();
  errors = 0;
  warnings = 0;
  gfc_buffer_error (false);
}


/* Set the flag for buffering errors or not.  */

void
gfc_buffer_error (bool flag)
{
  buffered_p = flag;
}


/* Add a single character to the error buffer or output depending on
   buffered_p.  */

static void
error_char (char c)
{
  if (buffered_p)
    {
      if (cur_error_buffer->index >= cur_error_buffer->allocated)
	{
	  cur_error_buffer->allocated = cur_error_buffer->allocated
				      ? cur_error_buffer->allocated * 2 : 1000;
	  cur_error_buffer->message = XRESIZEVEC (char, cur_error_buffer->message,
						  cur_error_buffer->allocated);
	}
      cur_error_buffer->message[cur_error_buffer->index++] = c;
    }
  else
    {
      if (c != 0)
	{
	  /* We build up complete lines before handing things
	     over to the library in order to speed up error printing.  */
	  static char *line;
	  static size_t allocated = 0, index = 0;

	  if (index + 1 >= allocated)
	    {
	      allocated = allocated ? allocated * 2 : 1000;
	      line = XRESIZEVEC (char, line, allocated);
	    }
	  line[index++] = c;
	  if (c == '\n')
	    {
	      line[index] = '\0';
	      fputs (line, stderr);
	      index = 0;
	    }
	}
    }
}


/* Copy a string to wherever it needs to go.  */

static void
error_string (const char *p)
{
  while (*p)
    error_char (*p++);
}


/* Print a formatted integer to the error buffer or output.  */

#define IBUF_LEN 60

static void
error_uinteger (unsigned long int i)
{
  char *p, int_buf[IBUF_LEN];

  p = int_buf + IBUF_LEN - 1;
  *p-- = '\0';

  if (i == 0)
    *p-- = '0';

  while (i > 0)
    {
      *p-- = i % 10 + '0';
      i = i / 10;
    }

  error_string (p + 1);
}

static void
error_integer (long int i)
{
  unsigned long int u;

  if (i < 0)
    {
      u = (unsigned long int) -i;
      error_char ('-');
    }
  else
    u = i;

  error_uinteger (u);
}


static size_t
gfc_widechar_display_length (gfc_char_t c)
{
  if (gfc_wide_is_printable (c) || c == '\t')
    /* Printable ASCII character, or tabulation (output as a space).  */
    return 1;
  else if (c < ((gfc_char_t) 1 << 8))
    /* Displayed as \x??  */
    return 4;
  else if (c < ((gfc_char_t) 1 << 16))
    /* Displayed as \u????  */
    return 6;
  else
    /* Displayed as \U????????  */
    return 10;
}


/* Length of the ASCII representation of the wide string, escaping wide
   characters as print_wide_char_into_buffer() does.  */

static size_t
gfc_wide_display_length (const gfc_char_t *str)
{
  size_t i, len;

  for (i = 0, len = 0; str[i]; i++)
    len += gfc_widechar_display_length (str[i]);

  return len;
}

static int
print_wide_char_into_buffer (gfc_char_t c, char *buf)
{
  static const char xdigit[16] = { '0', '1', '2', '3', '4', '5', '6',
    '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

  if (gfc_wide_is_printable (c) || c == '\t')
    {
      buf[1] = '\0';
      /* Tabulation is output as a space.  */
      buf[0] = (unsigned char) (c == '\t' ? ' ' : c);
      return 1;
    }
  else if (c < ((gfc_char_t) 1 << 8))
    {
      buf[4] = '\0';
      buf[3] = xdigit[c & 0x0F];
      c = c >> 4;
      buf[2] = xdigit[c & 0x0F];

      buf[1] = 'x';
      buf[0] = '\\';
      return 4;
    }
  else if (c < ((gfc_char_t) 1 << 16))
    {
      buf[6] = '\0';
      buf[5] = xdigit[c & 0x0F];
      c = c >> 4;
      buf[4] = xdigit[c & 0x0F];
      c = c >> 4;
      buf[3] = xdigit[c & 0x0F];
      c = c >> 4;
      buf[2] = xdigit[c & 0x0F];

      buf[1] = 'u';
      buf[0] = '\\';
      return 6;
    }
  else
    {
      buf[10] = '\0';
      buf[9] = xdigit[c & 0x0F];
      c = c >> 4;
      buf[8] = xdigit[c & 0x0F];
      c = c >> 4;
      buf[7] = xdigit[c & 0x0F];
      c = c >> 4;
      buf[6] = xdigit[c & 0x0F];
      c = c >> 4;
      buf[5] = xdigit[c & 0x0F];
      c = c >> 4;
      buf[4] = xdigit[c & 0x0F];
      c = c >> 4;
      buf[3] = xdigit[c & 0x0F];
      c = c >> 4;
      buf[2] = xdigit[c & 0x0F];

      buf[1] = 'U';
      buf[0] = '\\';
      return 10;
    }
}

static char wide_char_print_buffer[11];

const char *
gfc_print_wide_char (gfc_char_t c)
{
  print_wide_char_into_buffer (c, wide_char_print_buffer);
  return wide_char_print_buffer;
}


/* Show the file, where it was included, and the source line, give a
   locus.  Calls error_printf() recursively, but the recursion is at
   most one level deep.  */

static void error_printf (const char *, ...) ATTRIBUTE_GCC_GFC(1,2);

static void
show_locus (locus *loc, int c1, int c2)
{
  gfc_linebuf *lb;
  gfc_file *f;
  gfc_char_t *p;
  int i, offset, cmax;

  /* TODO: Either limit the total length and number of included files
     displayed or add buffering of arbitrary number of characters in
     error messages.  */

  /* Write out the error header line, giving the source file and error
     location (in GNU standard "[file]:[line].[column]:" format),
     followed by an "included by" stack and a blank line.  This header
     format is matched by a testsuite parser defined in
     lib/gfortran-dg.exp.  */

  lb = loc->lb;
  f = lb->file;

  error_string (f->filename);
  error_char (':');

  error_integer (LOCATION_LINE (lb->location));

  if ((c1 > 0) || (c2 > 0))
    error_char ('.');

  if (c1 > 0)
    error_integer (c1);

  if ((c1 > 0) && (c2 > 0))
    error_char ('-');

  if (c2 > 0)
    error_integer (c2);

  error_char (':');
  error_char ('\n');

  for (;;)
    {
      i = f->inclusion_line;

      f = f->up;
      if (f == NULL) break;

      error_printf ("    Included at %s:%d:", f->filename, i);
    }

  error_char ('\n');

  /* Calculate an appropriate horizontal offset of the source line in
     order to get the error locus within the visible portion of the
     line.  Note that if the margin of 5 here is changed, the
     corresponding margin of 10 in show_loci should be changed.  */

  offset = 0;

  /* If the two loci would appear in the same column, we shift
     '2' one column to the right, so as to print '12' rather than
     just '1'.  We do this here so it will be accounted for in the
     margin calculations.  */

  if (c1 == c2)
    c2 += 1;

  cmax = (c1 < c2) ? c2 : c1;
  if (cmax > terminal_width - 5)
    offset = cmax - terminal_width + 5;

  /* Show the line itself, taking care not to print more than what can
     show up on the terminal.  Tabs are converted to spaces, and
     nonprintable characters are converted to a "\xNN" sequence.  */

  p = &(lb->line[offset]);
  i = gfc_wide_display_length (p);
  if (i > terminal_width)
    i = terminal_width - 1;

  while (i > 0)
    {
      static char buffer[11];
      i -= print_wide_char_into_buffer (*p++, buffer);
      error_string (buffer);
    }

  error_char ('\n');

  /* Show the '1' and/or '2' corresponding to the column of the error
     locus.  Note that a value of -1 for c1 or c2 will simply cause
     the relevant number not to be printed.  */

  c1 -= offset;
  c2 -= offset;
  cmax -= offset;

  p = &(lb->line[offset]);
  for (i = 0; i < cmax; i++)
    {
      int spaces, j;
      spaces = gfc_widechar_display_length (*p++);

      if (i == c1)
	error_char ('1'), spaces--;
      else if (i == c2)
	error_char ('2'), spaces--;

      for (j = 0; j < spaces; j++)
	error_char (' ');
    }

  if (i == c1)
    error_char ('1');
  else if (i == c2)
    error_char ('2');

  error_char ('\n');

}


/* As part of printing an error, we show the source lines that caused
   the problem.  We show at least one, and possibly two loci; the two
   loci may or may not be on the same source line.  */

static void
show_loci (locus *l1, locus *l2)
{
  int m, c1, c2;

  if (l1 == NULL || l1->lb == NULL)
    {
      error_printf ("<During initialization>\n");
      return;
    }

  /* While calculating parameters for printing the loci, we consider possible
     reasons for printing one per line.  If appropriate, print the loci
     individually; otherwise we print them both on the same line.  */

  c1 = l1->nextc - l1->lb->line;
  if (l2 == NULL)
    {
      show_locus (l1, c1, -1);
      return;
    }

  c2 = l2->nextc - l2->lb->line;

  if (c1 < c2)
    m = c2 - c1;
  else
    m = c1 - c2;

  /* Note that the margin value of 10 here needs to be less than the
     margin of 5 used in the calculation of offset in show_locus.  */

  if (l1->lb != l2->lb || m > terminal_width - 10)
    {
      show_locus (l1, c1, -1);
      show_locus (l2, -1, c2);
      return;
    }

  show_locus (l1, c1, c2);

  return;
}


/* Workhorse for the error printing subroutines.  This subroutine is
   inspired by g77's error handling and is similar to printf() with
   the following %-codes:

   %c Character, %d or %i Integer, %s String, %% Percent
   %L  Takes locus argument
   %C  Current locus (no argument)

   If a locus pointer is given, the actual source line is printed out
   and the column is indicated.  Since we want the error message at
   the bottom of any source file information, we must scan the
   argument list twice -- once to determine whether the loci are
   present and record this for printing, and once to print the error
   message after and loci have been printed.  A maximum of two locus
   arguments are permitted.

   This function is also called (recursively) by show_locus in the
   case of included files; however, as show_locus does not resupply
   any loci, the recursion is at most one level deep.  */

#define MAX_ARGS 10

static void ATTRIBUTE_GCC_GFC(2,0)
error_print (const char *type, const char *format0, va_list argp)
{
  enum { TYPE_CURRENTLOC, TYPE_LOCUS, TYPE_INTEGER, TYPE_UINTEGER,
         TYPE_LONGINT, TYPE_ULONGINT, TYPE_CHAR, TYPE_STRING,
	 NOTYPE };
  struct
  {
    int type;
    int pos;
    union
    {
      int intval;
      unsigned int uintval;
      long int longintval;
      unsigned long int ulongintval;
      char charval;
      const char * stringval;
    } u;
  } arg[MAX_ARGS], spec[MAX_ARGS];
  /* spec is the array of specifiers, in the same order as they
     appear in the format string.  arg is the array of arguments,
     in the same order as they appear in the va_list.  */

  char c;
  int i, n, have_l1, pos, maxpos;
  locus *l1, *l2, *loc;
  const char *format;

  loc = l1 = l2 = NULL;

  have_l1 = 0;
  pos = -1;
  maxpos = -1;

  n = 0;
  format = format0;

  for (i = 0; i < MAX_ARGS; i++)
    {
      arg[i].type = NOTYPE;
      spec[i].pos = -1;
    }

  /* First parse the format string for position specifiers.  */
  while (*format)
    {
      c = *format++;
      if (c != '%')
	continue;

      if (*format == '%')
	{
	  format++;
	  continue;
	}

      if (ISDIGIT (*format))
	{
	  /* This is a position specifier.  For example, the number
	     12 in the format string "%12$d", which specifies the third
	     argument of the va_list, formatted in %d format.
	     For details, see "man 3 printf".  */
	  pos = atoi(format) - 1;
	  gcc_assert (pos >= 0);
	  while (ISDIGIT(*format))
	    format++;
	  gcc_assert (*format == '$');
	  format++;
	}
      else
	pos++;

      c = *format++;

      if (pos > maxpos)
	maxpos = pos;

      switch (c)
	{
	  case 'C':
	    arg[pos].type = TYPE_CURRENTLOC;
	    break;

	  case 'L':
	    arg[pos].type = TYPE_LOCUS;
	    break;

	  case 'd':
	  case 'i':
	    arg[pos].type = TYPE_INTEGER;
	    break;

	  case 'u':
	    arg[pos].type = TYPE_UINTEGER;
	    break;

	  case 'l':
	    c = *format++;
	    if (c == 'u')
	      arg[pos].type = TYPE_ULONGINT;
	    else if (c == 'i' || c == 'd')
	      arg[pos].type = TYPE_LONGINT;
	    else
	      gcc_unreachable ();
	    break;

	  case 'c':
	    arg[pos].type = TYPE_CHAR;
	    break;

	  case 's':
	    arg[pos].type = TYPE_STRING;
	    break;

	  default:
	    gcc_unreachable ();
	}

      spec[n++].pos = pos;
    }

  /* Then convert the values for each %-style argument.  */
  for (pos = 0; pos <= maxpos; pos++)
    {
      gcc_assert (arg[pos].type != NOTYPE);
      switch (arg[pos].type)
	{
	  case TYPE_CURRENTLOC:
	    loc = &gfc_current_locus;
	    /* Fall through.  */

	  case TYPE_LOCUS:
	    if (arg[pos].type == TYPE_LOCUS)
	      loc = va_arg (argp, locus *);

	    if (have_l1)
	      {
		l2 = loc;
		arg[pos].u.stringval = "(2)";
	      }
	    else
	      {
		l1 = loc;
		have_l1 = 1;
		arg[pos].u.stringval = "(1)";
	      }
	    break;

	  case TYPE_INTEGER:
	    arg[pos].u.intval = va_arg (argp, int);
	    break;

	  case TYPE_UINTEGER:
	    arg[pos].u.uintval = va_arg (argp, unsigned int);
	    break;

	  case TYPE_LONGINT:
	    arg[pos].u.longintval = va_arg (argp, long int);
	    break;

	  case TYPE_ULONGINT:
	    arg[pos].u.ulongintval = va_arg (argp, unsigned long int);
	    break;

	  case TYPE_CHAR:
	    arg[pos].u.charval = (char) va_arg (argp, int);
	    break;

	  case TYPE_STRING:
	    arg[pos].u.stringval = (const char *) va_arg (argp, char *);
	    break;

	  default:
	    gcc_unreachable ();
	}
    }

  for (n = 0; spec[n].pos >= 0; n++)
    spec[n].u = arg[spec[n].pos].u;

  /* Show the current loci if we have to.  */
  if (have_l1)
    show_loci (l1, l2);

  if (*type)
    {
      error_string (type);
      error_char (' ');
    }

  have_l1 = 0;
  format = format0;
  n = 0;

  for (; *format; format++)
    {
      if (*format != '%')
	{
	  error_char (*format);
	  continue;
	}

      format++;
      if (ISDIGIT (*format))
	{
	  /* This is a position specifier.  See comment above.  */
	  while (ISDIGIT (*format))
	    format++;

	  /* Skip over the dollar sign.  */
	  format++;
	}

      switch (*format)
	{
	case '%':
	  error_char ('%');
	  break;

	case 'c':
	  error_char (spec[n++].u.charval);
	  break;

	case 's':
	case 'C':		/* Current locus */
	case 'L':		/* Specified locus */
	  error_string (spec[n++].u.stringval);
	  break;

	case 'd':
	case 'i':
	  error_integer (spec[n++].u.intval);
	  break;

	case 'u':
	  error_uinteger (spec[n++].u.uintval);
	  break;

	case 'l':
	  format++;
	  if (*format == 'u')
	    error_uinteger (spec[n++].u.ulongintval);
	  else
	    error_integer (spec[n++].u.longintval);
	  break;

	}
    }

  error_char ('\n');
}


/* Wrapper for error_print().  */

static void
error_printf (const char *gmsgid, ...)
{
  va_list argp;

  va_start (argp, gmsgid);
  error_print ("", _(gmsgid), argp);
  va_end (argp);
}


/* Increment the number of errors, and check whether too many have
   been printed.  */

static void
gfc_increment_error_count (void)
{
  errors++;
  if ((gfc_option.max_errors != 0) && (errors >= gfc_option.max_errors))
    gfc_fatal_error ("Error count reached limit of %d.", gfc_option.max_errors);
}


/* Clear any output buffered in a pretty-print output_buffer.  */

static void
gfc_clear_pp_buffer (output_buffer *this_buffer)
{
  pretty_printer *pp = global_dc->printer;
  output_buffer *tmp_buffer = pp->buffer;
  pp->buffer = this_buffer;
  pp_clear_output_area (pp);
  pp->buffer = tmp_buffer;
  /* We need to reset last_location, otherwise we may skip caret lines
     when we actually give a diagnostic.  */
  global_dc->last_location = UNKNOWN_LOCATION;
}


/* Issue a warning.  */
/* Use gfc_warning instead, unless two locations are used in the same
   warning or for scanner.c, if the location is not properly set up.  */

void
gfc_warning_1 (const char *gmsgid, ...)
{
  va_list argp;

  if (inhibit_warnings)
    return;

  warning_buffer.flag = 1;
  warning_buffer.index = 0;
  cur_error_buffer = &warning_buffer;

  va_start (argp, gmsgid);
  error_print (_("Warning:"), _(gmsgid), argp);
  va_end (argp);

  error_char ('\0');

  if (!buffered_p)
  {
    warnings++;
    if (warnings_are_errors)
      gfc_increment_error_count();
  }
}


/* This is just a helper function to avoid duplicating the logic of
   gfc_warning.  */

static bool
gfc_warning (int opt, const char *gmsgid, va_list ap)
{
  va_list argp;
  va_copy (argp, ap);

  diagnostic_info diagnostic;
  bool fatal_errors = global_dc->fatal_errors;
  pretty_printer *pp = global_dc->printer;
  output_buffer *tmp_buffer = pp->buffer;

  gfc_clear_pp_buffer (pp_warning_buffer);

  if (buffered_p)
    {
      pp->buffer = pp_warning_buffer;
      global_dc->fatal_errors = false;
      /* To prevent -fmax-errors= triggering.  */
      --werrorcount;
    }

  diagnostic_set_info (&diagnostic, gmsgid, &argp, UNKNOWN_LOCATION,
		       DK_WARNING);
  diagnostic.option_index = opt;
  bool ret = report_diagnostic (&diagnostic);

  if (buffered_p)
    {
      pp->buffer = tmp_buffer;
      global_dc->fatal_errors = fatal_errors;

      warningcount_buffered = 0;
      werrorcount_buffered = 0;
      /* Undo the above --werrorcount if not Werror, otherwise
	 werrorcount is correct already.  */
      if (!ret)
	++werrorcount;
      else if (diagnostic.kind == DK_ERROR)
	++werrorcount_buffered;
      else
	++werrorcount, --warningcount, ++warningcount_buffered;
    }

  va_end (argp);
  return ret;
}

/* Issue a warning.  */
/* This function uses the common diagnostics, but does not support
   two locations; when being used in scanner.c, ensure that the location
   is properly setup. Otherwise, use gfc_warning_1.   */

bool
gfc_warning (int opt, const char *gmsgid, ...)
{
  va_list argp;

  va_start (argp, gmsgid);
  bool ret = gfc_warning (opt, gmsgid, argp);
  va_end (argp);
  return ret;
}


/* Whether, for a feature included in a given standard set (GFC_STD_*),
   we should issue an error or a warning, or be quiet.  */

notification
gfc_notification_std (int std)
{
  bool warning;

  warning = ((gfc_option.warn_std & std) != 0) && !inhibit_warnings;
  if ((gfc_option.allow_std & std) != 0 && !warning)
    return SILENT;

  return warning ? WARNING : ERROR;
}


/* Possibly issue a warning/error about use of a nonstandard (or deleted)
   feature.  An error/warning will be issued if the currently selected
   standard does not contain the requested bits.  Return false if
   an error is generated.  */

bool
gfc_notify_std_1 (int std, const char *gmsgid, ...)
{
  va_list argp;
  bool warning;
  const char *msg1, *msg2;
  char *buffer;

  warning = ((gfc_option.warn_std & std) != 0) && !inhibit_warnings;
  if ((gfc_option.allow_std & std) != 0 && !warning)
    return true;

  if (suppress_errors)
    return warning ? true : false;

  cur_error_buffer = warning ? &warning_buffer : &error_buffer;
  cur_error_buffer->flag = 1;
  cur_error_buffer->index = 0;

  if (warning)
    msg1 = _("Warning:");
  else
    msg1 = _("Error:");

  switch (std)
  {
    case GFC_STD_F2008_TS:
      msg2 = "TS 29113/TS 18508:";
      break;
    case GFC_STD_F2008_OBS:
      msg2 = _("Fortran 2008 obsolescent feature:");
      break;
    case GFC_STD_F2008:
      msg2 = "Fortran 2008:";
      break;
    case GFC_STD_F2003:
      msg2 = "Fortran 2003:";
      break;
    case GFC_STD_GNU:
      msg2 = _("GNU Extension:");
      break;
    case GFC_STD_LEGACY:
      msg2 = _("Legacy Extension:");
      break;
    case GFC_STD_F95_OBS:
      msg2 = _("Obsolescent feature:");
      break;
    case GFC_STD_F95_DEL:
      msg2 = _("Deleted feature:");
      break;
    default:
      gcc_unreachable ();
  }

  buffer = (char *) alloca (strlen (msg1) + strlen (msg2) + 2);
  strcpy (buffer, msg1);
  strcat (buffer, " ");
  strcat (buffer, msg2);

  va_start (argp, gmsgid);
  error_print (buffer, _(gmsgid), argp);
  va_end (argp);

  error_char ('\0');

  if (!buffered_p)
    {
      if (warning && !warnings_are_errors)
	warnings++;
      else
	gfc_increment_error_count();
      cur_error_buffer->flag = 0;
    }

  return (warning && !warnings_are_errors) ? true : false;
}


bool
gfc_notify_std (int std, const char *gmsgid, ...)
{
  va_list argp;
  bool warning;
  const char *msg, *msg2;
  char *buffer;

  warning = ((gfc_option.warn_std & std) != 0) && !inhibit_warnings;
  if ((gfc_option.allow_std & std) != 0 && !warning)
    return true;

  if (suppress_errors)
    return warning ? true : false;

  switch (std)
  {
    case GFC_STD_F2008_TS:
      msg = "TS 29113/TS 18508:";
      break;
    case GFC_STD_F2008_OBS:
      msg = _("Fortran 2008 obsolescent feature:");
      break;
    case GFC_STD_F2008:
      msg = "Fortran 2008:";
      break;
    case GFC_STD_F2003:
      msg = "Fortran 2003:";
      break;
    case GFC_STD_GNU:
      msg = _("GNU Extension:");
      break;
    case GFC_STD_LEGACY:
      msg = _("Legacy Extension:");
      break;
    case GFC_STD_F95_OBS:
      msg = _("Obsolescent feature:");
      break;
    case GFC_STD_F95_DEL:
      msg = _("Deleted feature:");
      break;
    default:
      gcc_unreachable ();
  }

  msg2 = _(gmsgid);
  buffer = (char *) alloca (strlen (msg) + strlen (msg2) + 2);
  strcpy (buffer, msg);
  strcat (buffer, " ");
  strcat (buffer, msg2);

  va_start (argp, gmsgid);
  if (warning)
    gfc_warning (0, buffer, argp);
  else
    gfc_error (buffer, argp);
  va_end (argp);

  return (warning && !warnings_are_errors) ? true : false;
}


/* Immediate warning (i.e. do not buffer the warning).  */
/* Use gfc_warning_now instead, unless two locations are used in the same
   warning or for scanner.c, if the location is not properly set up.  */

void
gfc_warning_now_1 (const char *gmsgid, ...)
{
  va_list argp;
  bool buffered_p_saved;

  if (inhibit_warnings)
    return;

  buffered_p_saved = buffered_p;
  buffered_p = false;
  warnings++;

  va_start (argp, gmsgid);
  error_print (_("Warning:"), _(gmsgid), argp);
  va_end (argp);

  error_char ('\0');

  if (warnings_are_errors)
    gfc_increment_error_count();

  buffered_p = buffered_p_saved;
}

/* Called from output_format -- during diagnostic message processing
   to handle Fortran specific format specifiers with the following meanings:

   %C  Current locus (no argument)
   %L  Takes locus argument
*/
static bool
gfc_format_decoder (pretty_printer *pp,
		    text_info *text, const char *spec,
		    int precision ATTRIBUTE_UNUSED, bool wide ATTRIBUTE_UNUSED,
		    bool plus ATTRIBUTE_UNUSED, bool hash ATTRIBUTE_UNUSED)
{
  switch (*spec)
    {
    case 'C':
    case 'L':
      {
	static const char *result = "(1)";
	locus *loc;
	if (*spec == 'C')
	  loc = &gfc_current_locus;
	else
	  loc = va_arg (*text->args_ptr, locus *);
	gcc_assert (loc->nextc - loc->lb->line >= 0);
	unsigned int offset = loc->nextc - loc->lb->line;
	gcc_assert (text->locus);
	*text->locus
	  = linemap_position_for_loc_and_offset (line_table,
						 loc->lb->location,
						 offset);
	global_dc->caret_char = '1';
	pp_string (pp, result);
	return true;
      }
    default:
      return false;
    }
}

/* Return a malloc'd string describing a location.  The caller is
   responsible for freeing the memory.  */
static char *
gfc_diagnostic_build_prefix (diagnostic_context *context,
			     const diagnostic_info *diagnostic)
{
  static const char *const diagnostic_kind_text[] = {
#define DEFINE_DIAGNOSTIC_KIND(K, T, C) (T),
#include "gfc-diagnostic.def"
#undef DEFINE_DIAGNOSTIC_KIND
    "must-not-happen"
  };
  static const char *const diagnostic_kind_color[] = {
#define DEFINE_DIAGNOSTIC_KIND(K, T, C) (C),
#include "gfc-diagnostic.def"
#undef DEFINE_DIAGNOSTIC_KIND
    NULL
  };
  gcc_assert (diagnostic->kind < DK_LAST_DIAGNOSTIC_KIND);
  const char *text = _(diagnostic_kind_text[diagnostic->kind]);
  const char *text_cs = "", *text_ce = "";
  pretty_printer *pp = context->printer;

  if (diagnostic_kind_color[diagnostic->kind])
    {
      text_cs = colorize_start (pp_show_color (pp),
				diagnostic_kind_color[diagnostic->kind]);
      text_ce = colorize_stop (pp_show_color (pp));
    }
  return build_message_string ("%s%s:%s ", text_cs, text, text_ce);
}

/* Return a malloc'd string describing a location.  The caller is
   responsible for freeing the memory.  */
static char *
gfc_diagnostic_build_locus_prefix (diagnostic_context *context,
				   const diagnostic_info *diagnostic)
{
  pretty_printer *pp = context->printer;
  const char *locus_cs = colorize_start (pp_show_color (pp), "locus");
  const char *locus_ce = colorize_stop (pp_show_color (pp));
  expanded_location s = diagnostic_expand_location (diagnostic);
  return (s.file == NULL
	  ? build_message_string ("%s%s:%s", locus_cs, progname, locus_ce )
	  : !strcmp (s.file, N_("<built-in>"))
	  ? build_message_string ("%s%s:%s", locus_cs, s.file, locus_ce)
	  : context->show_column
	  ? build_message_string ("%s%s:%d:%d:%s", locus_cs, s.file, s.line,
				  s.column, locus_ce)
	  : build_message_string ("%s%s:%d:%s", locus_cs, s.file, s.line, locus_ce));
}

static void
gfc_diagnostic_starter (diagnostic_context *context,
			diagnostic_info *diagnostic)
{
  char * locus_prefix = gfc_diagnostic_build_locus_prefix (context, diagnostic);
  char * prefix = gfc_diagnostic_build_prefix (context, diagnostic);
  /* First we assume there is a caret line.  */
  pp_set_prefix (context->printer, NULL);
  if (pp_needs_newline (context->printer))
    pp_newline (context->printer);
  pp_verbatim (context->printer, locus_prefix);
  /* Fortran uses an empty line between locus and caret line.  */
  pp_newline (context->printer);
  diagnostic_show_locus (context, diagnostic);
  if (pp_needs_newline (context->printer))
    {
      pp_newline (context->printer);
      /* If the caret line was shown, the prefix does not contain the
	 locus.  */
      pp_set_prefix (context->printer, prefix);
    }
  else
    {
      /* Otherwise, start again.  */
      pp_clear_output_area(context->printer);
      pp_set_prefix (context->printer, concat (locus_prefix, " ", prefix, NULL));
      free (prefix);
    }
  free (locus_prefix);
}

static void
gfc_diagnostic_finalizer (diagnostic_context *context,
			  diagnostic_info *diagnostic ATTRIBUTE_UNUSED)
{
  pp_destroy_prefix (context->printer);
  pp_newline_and_flush (context->printer);
}

/* Immediate warning (i.e. do not buffer the warning).  */
/* This function uses the common diagnostics, but does not support
   two locations; when being used in scanner.c, ensure that the location
   is properly setup. Otherwise, use gfc_warning_now_1.   */

bool
gfc_warning_now (int opt, const char *gmsgid, ...)
{
  va_list argp;
  diagnostic_info diagnostic;
  bool ret;

  va_start (argp, gmsgid);
  diagnostic_set_info (&diagnostic, gmsgid, &argp, UNKNOWN_LOCATION,
		       DK_WARNING);
  diagnostic.option_index = opt;
  ret = report_diagnostic (&diagnostic);
  va_end (argp);
  return ret;
}


/* Immediate error (i.e. do not buffer).  */
/* This function uses the common diagnostics, but does not support
   two locations; when being used in scanner.c, ensure that the location
   is properly setup. Otherwise, use gfc_error_now_1.   */

void
gfc_error_now (const char *gmsgid, ...)
{
  va_list argp;
  diagnostic_info diagnostic;

  va_start (argp, gmsgid);
  diagnostic_set_info (&diagnostic, gmsgid, &argp, UNKNOWN_LOCATION, DK_ERROR);
  report_diagnostic (&diagnostic);
  va_end (argp);
}


/* Fatal error, never returns.  */

void
gfc_fatal_error (const char *gmsgid, ...)
{
  va_list argp;
  diagnostic_info diagnostic;

  va_start (argp, gmsgid);
  diagnostic_set_info (&diagnostic, gmsgid, &argp, UNKNOWN_LOCATION, DK_FATAL);
  report_diagnostic (&diagnostic);
  va_end (argp);

  gcc_unreachable ();
}

/* Clear the warning flag.  */

void
gfc_clear_warning (void)
{
  warning_buffer.flag = 0;

  gfc_clear_pp_buffer (pp_warning_buffer);
  warningcount_buffered = 0;
  werrorcount_buffered = 0;
}


/* Check to see if any warnings have been saved.
   If so, print the warning.  */

void
gfc_warning_check (void)
{
  if (warning_buffer.flag)
    {
      warnings++;
      if (warning_buffer.message != NULL)
	fputs (warning_buffer.message, stderr);
      gfc_clear_warning ();
    }
  /* This is for the new diagnostics machinery.  */
  else if (! gfc_output_buffer_empty_p (pp_warning_buffer))
    {
      pretty_printer *pp = global_dc->printer;
      output_buffer *tmp_buffer = pp->buffer;
      pp->buffer = pp_warning_buffer;
      pp_really_flush (pp);
      warningcount += warningcount_buffered;
      werrorcount += werrorcount_buffered;
      gcc_assert (warningcount_buffered + werrorcount_buffered == 1);
      diagnostic_action_after_output (global_dc,
				      warningcount_buffered
				      ? DK_WARNING : DK_ERROR);
      pp->buffer = tmp_buffer;
    }
}


/* Issue an error.  */
/* Use gfc_error instead, unless two locations are used in the same
   warning or for scanner.c, if the location is not properly set up.  */

void
gfc_error_1 (const char *gmsgid, ...)
{
  va_list argp;

  if (warnings_not_errors)
    goto warning;

  if (suppress_errors)
    return;

  error_buffer.flag = 1;
  error_buffer.index = 0;
  cur_error_buffer = &error_buffer;

  va_start (argp, gmsgid);
  error_print (_("Error:"), _(gmsgid), argp);
  va_end (argp);

  error_char ('\0');

  if (!buffered_p)
    gfc_increment_error_count();

  return;

warning:

  if (inhibit_warnings)
    return;

  warning_buffer.flag = 1;
  warning_buffer.index = 0;
  cur_error_buffer = &warning_buffer;

  va_start (argp, gmsgid);
  error_print (_("Warning:"), _(gmsgid), argp);
  va_end (argp);

  error_char ('\0');

  if (!buffered_p)
  {
    warnings++;
    if (warnings_are_errors)
      gfc_increment_error_count();
  }
}

/* Issue an error.  */
/* This function uses the common diagnostics, but does not support
   two locations; when being used in scanner.c, ensure that the location
   is properly setup. Otherwise, use gfc_error_1.   */

static void
gfc_error (const char *gmsgid, va_list ap)
{
  va_list argp;
  va_copy (argp, ap);
  bool saved_abort_on_error = false;

  if (warnings_not_errors)
    {
      gfc_warning (/*opt=*/0, gmsgid, argp);
      va_end (argp);
      return;
    }

  if (suppress_errors)
    {
      va_end (argp);
      return;
    }

  diagnostic_info diagnostic;
  bool fatal_errors = global_dc->fatal_errors;
  pretty_printer *pp = global_dc->printer;
  output_buffer *tmp_buffer = pp->buffer;

  gfc_clear_pp_buffer (pp_error_buffer);

  if (buffered_p)
    {
      /* To prevent -dH from triggering an abort on a buffered error,
	 save abort_on_error and restore it below.  */
      saved_abort_on_error = global_dc->abort_on_error;
      global_dc->abort_on_error = false;
      pp->buffer = pp_error_buffer;
      global_dc->fatal_errors = false;
      /* To prevent -fmax-errors= triggering, we decrease it before
	 report_diagnostic increases it.  */
      --errorcount;
    }

  diagnostic_set_info (&diagnostic, gmsgid, &argp, UNKNOWN_LOCATION, DK_ERROR);
  report_diagnostic (&diagnostic);

  if (buffered_p)
    {
      pp->buffer = tmp_buffer;
      global_dc->fatal_errors = fatal_errors;
      global_dc->abort_on_error = saved_abort_on_error;

    }

  va_end (argp);
}


void
gfc_error (const char *gmsgid, ...)
{
  va_list argp;
  va_start (argp, gmsgid);
  gfc_error (gmsgid, argp);
  va_end (argp);
}


/* Immediate error.  */
/* Use gfc_error_now instead, unless two locations are used in the same
   warning or for scanner.c, if the location is not properly set up.  */

void
gfc_error_now_1 (const char *gmsgid, ...)
{
  va_list argp;
  bool buffered_p_saved;

  error_buffer.flag = 1;
  error_buffer.index = 0;
  cur_error_buffer = &error_buffer;

  buffered_p_saved = buffered_p;
  buffered_p = false;

  va_start (argp, gmsgid);
  error_print (_("Error:"), _(gmsgid), argp);
  va_end (argp);

  error_char ('\0');

  gfc_increment_error_count();

  buffered_p = buffered_p_saved;

  if (flag_fatal_errors)
    exit (FATAL_EXIT_CODE);
}


/* This shouldn't happen... but sometimes does.  */

void
gfc_internal_error (const char *gmsgid, ...)
{
  int e, w;
  va_list argp;
  diagnostic_info diagnostic;

  gfc_get_errors (&w, &e);
  if (e > 0)
    exit(EXIT_FAILURE);

  va_start (argp, gmsgid);
  diagnostic_set_info (&diagnostic, gmsgid, &argp, UNKNOWN_LOCATION, DK_ICE);
  report_diagnostic (&diagnostic);
  va_end (argp);

  gcc_unreachable ();
}


/* Clear the error flag when we start to compile a source line.  */

void
gfc_clear_error (void)
{
  error_buffer.flag = 0;
  warnings_not_errors = false;
  gfc_clear_pp_buffer (pp_error_buffer);
}


/* Tests the state of error_flag.  */

bool
gfc_error_flag_test (void)
{
  return error_buffer.flag
    || !gfc_output_buffer_empty_p (pp_error_buffer);
}


/* Check to see if any errors have been saved.
   If so, print the error.  Returns the state of error_flag.  */

bool
gfc_error_check (void)
{
  bool error_raised = (bool) error_buffer.flag;

  if (error_raised)
    {
      if (error_buffer.message != NULL)
	fputs (error_buffer.message, stderr);
      error_buffer.flag = 0;
      gfc_clear_pp_buffer (pp_error_buffer);

      gfc_increment_error_count();

      if (flag_fatal_errors)
	exit (FATAL_EXIT_CODE);
    }
  /* This is for the new diagnostics machinery.  */
  else if (! gfc_output_buffer_empty_p (pp_error_buffer))
    {
      error_raised = true;
      pretty_printer *pp = global_dc->printer;
      output_buffer *tmp_buffer = pp->buffer;
      pp->buffer = pp_error_buffer;
      pp_really_flush (pp);
      ++errorcount;
      gcc_assert (gfc_output_buffer_empty_p (pp_error_buffer));
      diagnostic_action_after_output (global_dc, DK_ERROR);
      pp->buffer = tmp_buffer;
    }

  return error_raised;
}

/* Move the text buffered from FROM to TO, then clear
   FROM. Independently if there was text in FROM, TO is also
   cleared. */

static void
gfc_move_output_buffer_from_to (output_buffer *from, output_buffer *to)
{
  gfc_clear_pp_buffer (to);
  /* We make sure this is always buffered.  */
  to->flush_p = false;

  if (! gfc_output_buffer_empty_p (from))
    {
      const char *str = output_buffer_formatted_text (from);
      output_buffer_append_r (to, str, strlen (str));
      gfc_clear_pp_buffer (from);
    }
}

/* Save the existing error state.  */

void
gfc_push_error (output_buffer *buffer_err, gfc_error_buf *err)
{
  err->flag = error_buffer.flag;
  if (error_buffer.flag)
    err->message = xstrdup (error_buffer.message);

  error_buffer.flag = 0;

  /* This part uses the common diagnostics.  */
  gfc_move_output_buffer_from_to (pp_error_buffer, buffer_err);
}


/* Restore a previous pushed error state.  */

void
gfc_pop_error (output_buffer *buffer_err, gfc_error_buf *err)
{
  error_buffer.flag = err->flag;
  if (error_buffer.flag)
    {
      size_t len = strlen (err->message) + 1;
      gcc_assert (len <= error_buffer.allocated);
      memcpy (error_buffer.message, err->message, len);
      free (err->message);
    }
  /* This part uses the common diagnostics.  */
  gfc_move_output_buffer_from_to (buffer_err, pp_error_buffer);
}


/* Free a pushed error state, but keep the current error state.  */

void
gfc_free_error (output_buffer *buffer_err, gfc_error_buf *err)
{
  if (err->flag)
    free (err->message);

  gfc_clear_pp_buffer (buffer_err);
}


/* Report the number of warnings and errors that occurred to the caller.  */

void
gfc_get_errors (int *w, int *e)
{
  if (w != NULL)
    *w = warnings + warningcount + werrorcount;
  if (e != NULL)
    *e = errors + errorcount + sorrycount + werrorcount;
}


/* Switch errors into warnings.  */

void
gfc_errors_to_warnings (bool f)
{
  warnings_not_errors = f;
}

void
gfc_diagnostics_init (void)
{
  diagnostic_starter (global_dc) = gfc_diagnostic_starter;
  diagnostic_finalizer (global_dc) = gfc_diagnostic_finalizer;
  diagnostic_format_decoder (global_dc) = gfc_format_decoder;
  global_dc->caret_char = '^';
  pp_warning_buffer = new (XNEW (output_buffer)) output_buffer ();
  pp_warning_buffer->flush_p = false;
  pp_error_buffer = new (XNEW (output_buffer)) output_buffer ();
  pp_error_buffer->flush_p = false;
}

void
gfc_diagnostics_finish (void)
{
  tree_diagnostics_defaults (global_dc);
  /* We still want to use the gfc starter and finalizer, not the tree
     defaults.  */
  diagnostic_starter (global_dc) = gfc_diagnostic_starter;
  diagnostic_finalizer (global_dc) = gfc_diagnostic_finalizer;
  global_dc->caret_char = '^';
}