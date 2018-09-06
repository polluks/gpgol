/* common.h - Common declarations for GpgOL
 * Copyright (C) 2004 Timo Schulz
 * Copyright (C) 2005, 2006, 2007, 2008 g10 Code GmbH
 * Copyright (C) 2015, 2016 by Bundesamt für Sicherheit in der Informationstechnik
 * Software engineering by Intevation GmbH
 *
 * This file is part of GpgOL.
 *
 * GpgOL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * GpgOL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GPGOL_COMMON_H
#define GPGOL_COMMON_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gpgme.h>

#include "common_indep.h"

#include <windows.h>

/* i18n stuff */
#include "w32-gettext.h"
#define _(a) gettext (a)
#define N_(a) gettext_noop (a)


/* Registry path to store plugin settings */
#define GPGOL_REGPATH "Software\\GNU\\GpgOL"

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif
extern HINSTANCE glob_hinst;
extern UINT      this_dll;

/*-- common.c --*/
void set_global_hinstance (HINSTANCE hinst);

char *get_data_dir (void);
char *get_gpg4win_dir (void);

int store_extension_value (const char *key, const char *val);
int store_extension_subkey_value (const char *subkey, const char *key,
                                  const char *val);
int load_extension_value (const char *key, char **val);

/* Get a temporary filename with and its name */
wchar_t *get_tmp_outfile (wchar_t *name, HANDLE *outHandle);

wchar_t *get_pretty_attachment_name (wchar_t *path, protocol_t protocol,
                                     int signature);

/*-- verify-dialog.c --*/
int verify_dialog_box (gpgme_protocol_t protocol,
                       gpgme_verify_result_t res,
                       const char *filename);


/*-- inspectors.cpp --*/
int initialize_inspectors (void);

#if __GNUC__ >= 4
# define GPGOL_GCC_A_SENTINEL(a) __attribute__ ((sentinel(a)))
#else
# define GPGOL_GCC_A_SENTINEL(a)
#endif


/*-- common.c --*/

void fatal_error (const char *format, ...);

char *read_w32_registry_string (const char *root, const char *dir,
                                const char *name);
char *percent_escape (const char *str, const char *extra);

void fix_linebreaks (char *str, int *len);

/* Format a date from gpgme (seconds since epoch)
   with windows system locale. */
char *format_date_from_gpgme (unsigned long time);

/* Get the name of the uiserver */
char *get_uiserver_name (void);

int is_elevated (void);

/*-- main.c --*/
void read_options (void);
int write_options (void);

extern int g_ol_version_major;

void bring_to_front (HWND wid);

int gpgol_message_box (HWND parent, const char *utf8_text,
                       const char *utf8_caption, UINT type);

/* Show a bug message with the code. */
void gpgol_bug (HWND parent, int code);

void i18n_init (void);
#define ERR_CRYPT_RESOLVER_FAILED 1
#define ERR_WANTS_SEND_MIME_BODY 2
#define ERR_WANTS_SEND_INLINE_BODY 3
#define ERR_INLINE_BODY_TO_BODY 4
#define ERR_INLINE_BODY_INV_STATE 5
#define ERR_SEND_FALLBACK_FAILED 6
#define ERR_GET_BASE_MSG_FAILED 7
#ifdef __cplusplus
}
#endif
#endif /*GPGOL_COMMON_H*/
