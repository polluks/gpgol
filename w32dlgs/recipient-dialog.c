/* recipient-dialog.c
 *	Copyright (C) 2004 Timo Schulz
 *	Copyright (C) 2005 g10 Code GmbH
 *
 * This file is part of GPGME Dialogs.
 *
 * GPGME Dialogs is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 
 * of the License, or (at your option) any later version.
 *  
 * GPGME Dialogs is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with GPGME Dialogs; if not, write to the Free Software Foundation, 
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
 */
#include <windows.h>
#include <commctrl.h>
#include <time.h>

#include "resource.h"
#include "gpgme.h"
#include "keycache.h"
#include "intern.h"

struct recipient_cb_s {
    keycache_t rset;    
    int opts;
};

struct key_item_s {
    char name [150];
    char e_mail[100];
    char key_info[64];
    char key_id[32];
    char validity[32];
};


static void
initialize_rsetbox(HWND hwnd)
{
    LVCOLUMN col;

    memset(&col, 0, sizeof col);
    col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;;
    col.pszText = "Name";
    col.cx = 100;
    col.iSubItem = 0;
    ListView_InsertColumn( hwnd, 0, &col );

    col.pszText = "E-Mail";
    col.cx = 100;
    col.iSubItem = 1;
    ListView_InsertColumn( hwnd, 1, &col );

    col.pszText = "Key-Info";
    col.cx = 110;
    col.iSubItem = 2;
    ListView_InsertColumn( hwnd, 2, &col );

    col.pszText = "Key ID";
    col.cx = 70;
    col.iSubItem = 3;
    ListView_InsertColumn( hwnd, 3, &col );

    col.pszText = "Validity";
    col.cx = 70;
    col.iSubItem = 4;
    ListView_InsertColumn( hwnd, 4, &col );

    ListView_SetExtendedListViewStyleEx( hwnd, 0, LVS_EX_FULLROWSELECT );
}


static void load_rsetbox(HWND hwnd)
{
    
    LVITEM lvi;
    DWORD val;
    char keybuf[128];
    const char * s;
    const char * trust_items[] = {
	"UNKNOWN",
	"UNDEFINED",
	"NEVER",
	"MARGINAL",
	"FULL",
	"ULTIMATE"
    };
    void *ctx=NULL;
    gpgme_key_t key=NULL;

    memset(&lvi, 0, sizeof (lvi));
    enum_gpg_keys(NULL, &ctx);
    while (!enum_gpg_keys(&key, &ctx)) {
	if( !gpgme_key_get_ulong_attr( key, GPGME_ATTR_CAN_ENCRYPT, NULL, 0 ) )
	    continue;
	/* check that the primary key is *not* revoked, expired or invalid */
	if( gpgme_key_get_ulong_attr( key, GPGME_ATTR_KEY_REVOKED, NULL, 0 )
         || gpgme_key_get_ulong_attr( key, GPGME_ATTR_KEY_EXPIRED, NULL, 0 )
	 || gpgme_key_get_ulong_attr( key, GPGME_ATTR_KEY_INVALID, NULL, 0 ) )
	    continue;
	ListView_InsertItem( hwnd, &lvi );

	s = gpgme_key_get_string_attr( key, GPGME_ATTR_NAME, NULL, 0 );
	ListView_SetItemText( hwnd, 0, 0, (char *)s );

	s = gpgme_key_get_string_attr( key, GPGME_ATTR_EMAIL, NULL, 0 );
	ListView_SetItemText( hwnd, 0, 1, (char *)s );

	s = gpgme_key_get_string_attr( key, GPGME_ATTR_ALGO, NULL, 0 );
	sprintf( keybuf, "%s", s );
	if( (s = gpgme_key_get_string_attr( key, GPGME_ATTR_ALGO, NULL, 1 )) )
	    sprintf( keybuf+strlen( keybuf ), "/%s", s );
	strcat( keybuf, " " );
	val = gpgme_key_get_ulong_attr( key, GPGME_ATTR_LEN, NULL, 0 );
	sprintf( keybuf+strlen( keybuf ), "%d", val );
	if( (val = gpgme_key_get_ulong_attr( key, GPGME_ATTR_LEN, NULL, 1 )) )
	    sprintf( keybuf+strlen( keybuf ), "/%d", val );
	s = keybuf;
	ListView_SetItemText( hwnd, 0, 2, (char *) s );

	s = gpgme_key_get_string_attr( key, GPGME_ATTR_KEYID, NULL, 0 ) + 8;
	ListView_SetItemText( hwnd, 0, 3, (char *)s );

	val = gpgme_key_get_ulong_attr( key, GPGME_ATTR_VALIDITY, NULL, 0 );
	if( val < 0 || val > 5 ) val = 0;
	s = (char *)trust_items[val];
	ListView_SetItemText( hwnd, 0, 4, (char *)s );
    }
}


static void
copy_item( HWND dlg, int id_from )
{
    HWND src, dst;
    LVITEM lvi;
    struct key_item_s from;
    int idx;

    src = GetDlgItem( dlg, id_from );	
    dst = GetDlgItem( dlg, id_from==IDC_ENC_RSET1 ?
		      IDC_ENC_RSET2 : IDC_ENC_RSET1 );

    idx = ListView_GetNextItem( src, -1, LVNI_SELECTED );
    if( idx == -1 )
	return;

    memset( &from, 0, sizeof from );
    ListView_GetItemText( src, idx, 0, from.name, sizeof from.name-1 );
    ListView_GetItemText( src, idx, 1, from.e_mail, sizeof from.e_mail-1 );
    ListView_GetItemText( src, idx, 2, from.key_info, sizeof from.key_info-1 );
    ListView_GetItemText( src, idx, 3, from.key_id, sizeof from.key_id-1 );
    ListView_GetItemText( src, idx, 4, from.validity, sizeof from.validity-1 );

    ListView_DeleteItem( src, idx );

    memset( &lvi, 0, sizeof lvi );
    ListView_InsertItem( dst, &lvi );
    ListView_SetItemText( dst, 0, 0, from.name );
    ListView_SetItemText( dst, 0, 1, from.e_mail );
    ListView_SetItemText( dst, 0, 2, from.key_info );
    ListView_SetItemText( dst, 0, 3, from.key_id );
    ListView_SetItemText( dst, 0, 4, from.validity );
}


BOOL CALLBACK
recipient_dlg_proc( HWND dlg, UINT msg, WPARAM wparam, LPARAM lparam )
{
    static struct recipient_cb_s * rset_cb;
    static int rset_state=1;
    NMHDR * notify;
    HWND hrset;
    BOOL flag;    
    int xy[2], i;

    switch( msg ) {
    case WM_INITDIALOG:
	rset_cb = (struct recipient_cb_s *)lparam;
	initialize_rsetbox( GetDlgItem( dlg, IDC_ENC_RSET1 ) );
	load_rsetbox( GetDlgItem( dlg, IDC_ENC_RSET1 ) );
	initialize_rsetbox( GetDlgItem( dlg, IDC_ENC_RSET2 ) );
	xy[0] = GetSystemMetrics( SM_CXSCREEN );
	xy[1] = GetSystemMetrics( SM_CYSCREEN );	
	SetWindowPos( dlg, NULL, xy[0]/3, xy[0]/3, 0, 0, 
		      SWP_NOSIZE|SWP_NOZORDER );
	SetForegroundWindow( dlg );
	return TRUE;

    case WM_SYSCOMMAND:
	if( wparam == SC_CLOSE )
	    EndDialog( dlg, TRUE );
	break;

    case WM_NOTIFY:
	notify = (LPNMHDR)lparam;
	if( notify 
	    && (notify->idFrom == IDC_ENC_RSET1
	    ||  notify->idFrom == IDC_ENC_RSET2)
	    && notify->code ==  NM_DBLCLK )
	    copy_item( dlg, notify->idFrom );
	break;

    case WM_COMMAND:
	switch( HIWORD(wparam) ) {
	case BN_CLICKED:
	    if( (int)LOWORD( wparam ) == IDC_ENC_OPTSYM ) {
		rset_state ^= 1;
		EnableWindow( GetDlgItem( dlg, IDC_ENC_RSET1 ), rset_state );
		EnableWindow( GetDlgItem( dlg, IDC_ENC_RSET2 ), rset_state );
		ListView_DeleteAllItems( GetDlgItem( dlg, IDC_ENC_RSET2 ) );
	    }
	    break;
	}
	switch( LOWORD( wparam ) ) {
	case IDOK:
	    flag = IsDlgButtonChecked( dlg, IDC_ENC_OPTARMOR );
	    if( flag )
		rset_cb->opts |= OPT_FLAG_ARMOR;
	    flag = IsDlgButtonChecked( dlg, IDC_ENC_OPTSYM );
	    if( flag ) {
		rset_cb->opts |= OPT_FLAG_SYMETRIC;
		EndDialog( dlg, TRUE );
	    }
	    keycache_new (&rset_cb->rset);
	    
	    hrset = GetDlgItem( dlg, IDC_ENC_RSET2 );
	    for( i=0; i < ListView_GetItemCount( hrset ); i++ ) {
		char keyid[32], valid[32];
		gpgme_key_t key;
		ListView_GetItemText( hrset, i, 3, keyid, sizeof keyid-1 );
		ListView_GetItemText( hrset, i, 4, valid, sizeof valid-1 );
		key = find_gpg_key(keyid);
		keycache_add(&rset_cb->rset, key);
		if( strcmp( valid, "FULL" ) && strcmp( valid, "ULTIMATE" ) )
		    rset_cb->opts |= OPT_FLAG_FORCE;
	    }
	    EndDialog( dlg, TRUE );
	    break;

	case IDCANCEL:
	    rset_cb->opts = OPT_FLAG_CANCEL;
	    rset_cb->rset = NULL;
	    EndDialog( dlg, FALSE );
	    break;
	}
	break;
    }
    return FALSE;
}


static gpgme_key_t* keycache_to_key_array(keycache_t ctx)
{
    keycache_t t;
    int n = keycache_size(ctx), i=0;
    gpgme_key_t *keys = calloc(n+1, sizeof (gpgme_key_t));
    
    for (t=ctx; t; t = t->next)
	keys[i++] = t->key;
    keys[i] = NULL; /*eof*/
    return keys;
}


int recipient_dialog_box(gpgme_key_t **ret_rset, int *ret_opts)
{
    struct recipient_cb_s cb;

    memset (&cb, 0, sizeof (cb));
    DialogBoxParam (glob_hinst, (LPCTSTR)IDD_ENC, GetDesktopWindow(),
		    recipient_dlg_proc, (LPARAM)&cb);
    if (cb.opts & OPT_FLAG_CANCEL) {
	*ret_rset = NULL;
	*ret_opts = OPT_FLAG_CANCEL;
    }
    else {
	*ret_rset = keycache_to_key_array(cb.rset);
	*ret_opts = cb.opts;
	keycache_free(cb.rset);
    }
    return 0;
}