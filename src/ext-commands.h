/* ext-commands.h - Definitions for our subclass of IExchExtCommands
 *	Copyright (C) 2005, 2007 g10 Code GmbH
 * 
 * This file is part of GpgOL.
 * 
 * GpgOL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * GpgOL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef EXT_COMMANDS_H
#define EXT_COMMANDS_H


/*
   GpgolExtCommands 

   Makes the menu and toolbar extensions. Implements the own commands.
 */
class GpgolExtCommands : public IExchExtCommands
{
public:
  GpgolExtCommands (GpgolExt* pParentInterface);
  
private:
  ULONG m_lRef;
  ULONG m_lContext;
  
  UINT  m_nCmdEncrypt;
  UINT  m_nCmdSign;
  UINT  m_nCmdShowInfo;
  UINT  m_nCmdCheckSig;

  UINT  m_nToolbarButtonID1;
  UINT  m_nToolbarButtonID2;     
  UINT  m_nToolbarBitmap1;
  UINT  m_nToolbarBitmap2;
  
  HWND  m_hWnd;
  
  GpgolExt* m_pExchExt;
  
public:
  STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppvObj);
  inline STDMETHODIMP_(ULONG) AddRef (void)
  { 
    ++m_lRef;
    return m_lRef; 
  };
  inline STDMETHODIMP_(ULONG) Release (void) 
  {
    ULONG lCount = --m_lRef;
    if (!lCount) 
      delete this;
    return lCount;
  };

  STDMETHODIMP InstallCommands (LPEXCHEXTCALLBACK pEECB, HWND hWnd,
                                HMENU hMenu, UINT FAR * pnCommandIDBase,
                                LPTBENTRY pTBEArray,
                                UINT nTBECnt, ULONG lFlags);
  STDMETHODIMP DoCommand (LPEXCHEXTCALLBACK pEECB, UINT nCommandID);
  STDMETHODIMP_(void) InitMenu (LPEXCHEXTCALLBACK pEECB);
  STDMETHODIMP Help (LPEXCHEXTCALLBACK pEECB, UINT nCommandID);
  STDMETHODIMP QueryHelpText (UINT nCommandID, ULONG lFlags,
                              LPTSTR szText, UINT nCharCnt);
  STDMETHODIMP QueryButtonInfo (ULONG lToolbarID, UINT nToolbarButtonID, 
                                LPTBBUTTON pTBB, LPTSTR lpszDescription,
                                UINT nCharCnt, ULONG lFlags);
  STDMETHODIMP ResetToolbar (ULONG nToolbarID, ULONG lFlags);

  inline void SetContext (ULONG lContext)
  { 
    m_lContext = lContext;
  };
};



#endif /*EXT_COMMANDS_H*/
