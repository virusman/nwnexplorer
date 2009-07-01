// AtlRolloutCtrl.h
// Copyright (C) 2001 Bjoern Graf
// bjoern.graf@gmx.net
// All rights reserved.
//
// The code and information is provided "as-is" without
// warranty of any kind, either expressed or implied.
//

#if !defined(__ROLLOUTCTRL_H)
#define __ROLLOUTCTRL_H

/////////////////////////////////////////////////////////////////////////////
// CRolloutCtrlButton - 

// Styles
//#define RCS_

// Messages

typedef CWinTraits<WS_TABSTOP | WS_GROUP | WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0>        CRolloutCtrlButtonTraits;

template <class T, class TBase = CButton, class TWinTraits = CRolloutCtrlButtonTraits>
class ATL_NO_VTABLE CRolloutCtrlButtonImpl : public CWindowImpl<T, TBase, TWinTraits>
{
    public:
        DECLARE_WND_SUPERCLASS(NULL, TBase::GetWndClassName())

            // Internal states
            unsigned int m_fChecked:1;
        unsigned int m_fFocus:1;
        unsigned int m_fPressed:1;
        unsigned int m_fEnabled:1;

        // Construction
        CRolloutCtrlButtonImpl() : m_fChecked(1), m_fFocus(0), m_fPressed(0), m_fEnabled(0)
    {}

        // overridden to provide proper initialization
        HWND Create(HWND hWndParent, LPCTSTR szWindowName,
                    DWORD dwStyle = 0, DWORD dwExStyle = 0,
                    UINT nID = 0U, LPVOID lpCreateParam = NULL)
        {
            return CWindowImpl<T, TBase, TWinTraits>::Create(hWndParent, rcDefault, szWindowName,
                                                             TWinTraits::GetWndStyle(dwStyle), TWinTraits::GetWndExStyle(dwExStyle),
                                                             nID, lpCreateParam);
        }
        BOOL SubclassWindow(HWND hWnd)
        {
            BOOL bRet = CWindowImpl< T, TBase, TWinTraits>::SubclassWindow(hWnd);
            if(bRet)
                Init();
            return bRet;
        }

        // Attributes
        bool IsChecked()
        {
            return m_fChecked;
        }
        void SetChecked(bool fChecked)
        {
            m_fChecked = fChecked;
        }

        // Operations

        // Overrideables
        void DoPaint(CDCHandle dc)
        {
            RECT rect;
            RECT rcClient;
            GetClientRect(&rcClient);
            rect = rcClient;

            TCHAR szTitle[MAX_PATH];
            GetWindowText(szTitle, MAX_PATH);
            HFONT hOldFont = dc.SelectFont(GetFont());
            SIZE ptTextExtent;
            dc.GetTextExtent(szTitle, -1, &ptTextExtent);
            SIZE ptTextExtent2;
            dc.GetTextExtent(_T ("+"), -1, &ptTextExtent2);
            if (ptTextExtent .cx < ptTextExtent2 .cx)
                ptTextExtent .cx = ptTextExtent2 .cx;
            if (ptTextExtent .cy < ptTextExtent2 .cy)
                ptTextExtent .cy = ptTextExtent2 .cy;

            // draw text
            rect.top   += ::GetSystemMetrics(SM_CXBORDER);

            dc.DrawFrameControl(&rcClient, DFC_BUTTON, DFCS_BUTTONPUSH | DFCS_FLAT |
                                (m_fPressed ? DFCS_PUSHED : 0) | (m_fChecked ? DFCS_CHECKED : 0) | (m_fEnabled ? DFCS_INACTIVE : 0));
            if(m_fPressed)
                ::OffsetRect(&rect, 1, 1);
            dc.SetBkMode(TRANSPARENT);
            if(m_fEnabled)
                dc.DrawText(szTitle, -1, &rect, DT_CENTER | DT_TOP | DT_SINGLELINE);
            else
            {
                COLORREF crOldText = dc.SetTextColor(::GetSysColor(COLOR_3DHILIGHT));
                ::OffsetRect(&rect, 1, 1);
                dc.DrawText(szTitle, -1, &rect, DT_CENTER | DT_TOP | DT_SINGLELINE);
                dc.SetTextColor(::GetSysColor(COLOR_3DSHADOW));
                ::OffsetRect(&rect, -1, -1);
                dc.DrawText(szTitle, -1, &rect, DT_CENTER | DT_TOP | DT_SINGLELINE);
                dc.SetTextColor(crOldText);
            }

            // draw expand/collapse button
            if(m_fChecked)
                lstrcpy(szTitle, _T("-"));
            else
                lstrcpy(szTitle, _T("+"));
            rect.left += 2 * ::GetSystemMetrics(SM_CXEDGE);
            if(m_fEnabled)
                dc.DrawText(szTitle, -1, &rect, DT_LEFT | DT_TOP | DT_SINGLELINE);
            else
            {
                COLORREF crOldText = dc.SetTextColor(::GetSysColor(COLOR_3DHILIGHT));
                ::OffsetRect(&rect, 1, 1);
                dc.DrawText(szTitle, -1, &rect, DT_LEFT | DT_TOP | DT_SINGLELINE);
                dc.SetTextColor(::GetSysColor(COLOR_3DSHADOW));
                ::OffsetRect(&rect, -1, -1);
                dc.DrawText(szTitle, -1, &rect, DT_LEFT | DT_TOP | DT_SINGLELINE);
                dc.SetTextColor(crOldText);
            }

            dc.SelectFont(hOldFont);

            // draw focus rect
            if(m_fFocus)
            {
                ::CopyRect(&rect, &rcClient);
                ::InflateRect(&rect, -::GetSystemMetrics(SM_CXEDGE), -::GetSystemMetrics(SM_CYEDGE));
                dc.DrawFocusRect(&rect);
            }
        }

        // Message map and handlers
        typedef CRolloutCtrlButtonImpl< T, TBase, TWinTraits >  thisClass;
        BEGIN_MSG_MAP(thisClass)
            MESSAGE_HANDLER(WM_CREATE, OnCreate)
            MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
            MESSAGE_HANDLER(WM_PAINT, OnPaint)
            MESSAGE_HANDLER(WM_PRINTCLIENT, OnPaint)
            MESSAGE_HANDLER(WM_ENABLE, OnEnable)
            MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
            MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDblClk)
            MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
            MESSAGE_HANDLER(WM_CAPTURECHANGED, OnCaptureChanged)
            MESSAGE_HANDLER(WM_SETFOCUS, OnFocus)
            MESSAGE_HANDLER(WM_KILLFOCUS, OnFocus)
            MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
            MESSAGE_HANDLER(WM_KEYUP, OnKeyUp)
            MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
            END_MSG_MAP()

            LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
            {
                Init();
                bHandled = FALSE;
                return 1;
            }

        LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
        {
            return 1;   // no background needed
        }

        LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
        {
            T* pT = static_cast<T*>(this);
            if(wParam != NULL)
            {
                pT->DoPaint((HDC)wParam);
            }
            else
            {
                CPaintDC dc(m_hWnd);
                pT->DoPaint(dc.m_hDC);
            }
            return 0;
        }

        LRESULT OnEnable(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
        {
            m_fEnabled = wParam ? true : false;
            bHandled = FALSE;
            return 1;
        }

        LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
        {
            //          if(::GetCapture() == m_hWnd)
            {
                SetFocus();
                SetCapture();
                m_fPressed = 1;
                Invalidate();
                UpdateWindow();
            }
            return 0;
        }

        LRESULT OnLButtonDblClk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
        {
            if(::GetCapture() != m_hWnd)
                SetCapture();
            if(m_fPressed == 0)
            {
                m_fPressed = 1;
                Invalidate();
                UpdateWindow();
            }
            return 0;
        }

        LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
        {
            if(m_fPressed == 1)
            {
                m_fChecked = !m_fChecked;
                ::SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd);
            }
            ::ReleaseCapture();
            return 0;
        }

        LRESULT OnCaptureChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
        {
            if(m_fPressed == 1)
            {
                m_fPressed = 0;
                Invalidate();
                UpdateWindow();
            }
            bHandled = FALSE;
            return 1;
        }

        LRESULT OnFocus(UINT uMsg, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
        {
            m_fFocus = (uMsg == WM_SETFOCUS) ? 1 : 0;
            Invalidate();
            UpdateWindow();
            bHandled = FALSE;
            return 0;
        }

        LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
        {
            if(wParam == VK_SPACE && m_fPressed == 0)
            {
                m_fPressed = 1;
                Invalidate();
                UpdateWindow();
            }
            return 0;
        }

        LRESULT OnKeyUp(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
        {
            if(wParam == VK_SPACE && m_fPressed == 1)
            {
                m_fPressed = 0;
                m_fChecked = !m_fChecked;
                ::SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd);
                Invalidate();
                UpdateWindow();
            }
            return 0;
        }

        LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
        {
            if(::GetCapture() == m_hWnd)
            {
                POINT ptCursor = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                RECT  rc;
                GetClientRect(&rc);
                unsigned int uPressed = ::PtInRect(&rc, ptCursor) ? 1 : 0;
                if(m_fPressed != uPressed)
                {
                    m_fPressed = uPressed;
                    Invalidate();
                    UpdateWindow();
                }
            }
            bHandled = FALSE;
            return 1;
        }

        // Implementation
        void Init()
        {
            // We need this style to prevent Windows from painting the button
            ModifyStyle(0, WS_GROUP | WS_TABSTOP | BS_OWNERDRAW);

            m_fEnabled = IsWindowEnabled() ? true : false;
        }
};

class CRolloutCtrlButton : public CRolloutCtrlButtonImpl<CRolloutCtrlButton>
{
    public:
        DECLARE_WND_SUPERCLASS(_T("WTL_RolloutCtrlButton"), GetWndClassName())

            CRolloutCtrlButton()
            : CRolloutCtrlButtonImpl<CRolloutCtrlButton>()
            { }
};

/////////////////////////////////////////////////////////////////////////////
// CRolloutCtrl - 

// Styles
#define RCS_COLLAPSEDISABLED 0x00000001
//#define RCS_

// Messages
#define RCM_BASE                                        (WM_USER + 2)
#define RCM_TOGGLEEXPANDCOLLAPSE        (RCM_BASE + 0)
#define RCM_EXPAND                                      (RCM_BASE + 1)
#define RCM_COLLAPSE                            (RCM_BASE + 2)
#define RCM_ISEXPANDED                          (RCM_BASE + 3)
#define RCM_GETRECT                                     (RCM_BASE + 4)

template <DWORD t_dwStyle, DWORD t_dwExStyle, DWORD t_dwExRolloutCtrlStyle>
class CRolloutCtrlImplTraits
{
    public:
        static DWORD GetWndStyle(DWORD dwStyle)
        {
            return dwStyle == 0 ? t_dwStyle : dwStyle;
        }
        static DWORD GetWndExStyle(DWORD dwExStyle)
        {
            return dwExStyle == 0 ? t_dwExStyle : dwExStyle;
        }
        static DWORD GetExtendedRcStyle(DWORD dwExRcStyle)
        {
            return dwExRcStyle == 0 ? t_dwExRolloutCtrlStyle : dwExRcStyle;
        }
};

typedef CRolloutCtrlImplTraits<WS_TABSTOP | WS_GROUP | WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, WS_EX_CONTROLPARENT, RCS_COLLAPSEDISABLED>    CRolloutCtrlTraits;

template <class T, class TChild, class TBase = CButton, class TWinTraits = CRolloutCtrlTraits>
class ATL_NO_VTABLE CRolloutCtrlImpl : public CWindowImpl<T, TBase, TWinTraits>
{
    public:
        DECLARE_WND_SUPERCLASS(NULL, TBase::GetWndClassName())

            // Array of controls
            CContainedWindowT<TChild> m_Child;
        RECT m_rcExpanded;
        RECT m_rcCollapsed;
        RECT m_rcButton;
        SIZE m_sizeBorder;
        CRolloutCtrlButton m_button;

        // Rollout control specific extended styles
        DWORD m_dwExtendedStyle;

        // Internal states
        unsigned int m_fExpanded:1;
        unsigned int m_fFocus:1;
        unsigned int m_fPressed:1;
        unsigned int m_fEnabled:1;

        // Construction
        CRolloutCtrlImpl() : m_Child(this, 1), m_fExpanded(1), m_fFocus(0), m_fPressed(0), m_fEnabled(0)
    {}

        // overridden to provide proper initialization
        HWND Create(HWND hWndParent, LPCTSTR szWindowName,
                    DWORD dwStyle = 0, DWORD dwExStyle = 0, DWORD dwExRcStyle = 0,
                    UINT nID = 0U, LPVOID lpCreateParam = NULL)
        {
            m_dwExtendedStyle = TWinTraits::GetExtendedRcStyle(dwExRcStyle);
            return CWindowImpl<T, TBase, TWinTraits>::Create(hWndParent, rcDefault, szWindowName,
                                                             TWinTraits::GetWndStyle(dwStyle), TWinTraits::GetWndExStyle(dwExStyle),
                                                             nID, lpCreateParam);
        }
        BOOL SubclassWindow(HWND hWnd)
        {
            BOOL bRet = CWindowImpl< T, TBase, TWinTraits>::SubclassWindow(hWnd);
            if(bRet)
                Init();
            return bRet;
        }

        // Attributes
        bool IsExpanded()
        {
            return m_fExpanded;
        }

        // Operations
        TChild* operator ->()
        {
            return &m_Child;
        }

        void GetRect(bool fExpanded, RECT* pRect)
        {
            if(fExpanded)
                ::CopyRect(pRect, &m_rcExpanded);
            else
                ::CopyRect(pRect, &m_rcCollapsed);
        }

        void Expand(bool fExpand = true)
        {
            m_fExpanded = fExpand;
            if(m_fExpanded)
            {
                SetWindowPos(NULL, &m_rcExpanded, SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW);
                m_Child.ShowWindow(SW_SHOW);
            }
            else
            {
                SetWindowPos(NULL, &m_rcCollapsed, SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW);
                m_Child.ShowWindow(SW_HIDE);
            }
            m_button.SetChecked(m_fExpanded);
        }
        void ToggleExpandCollapse()
        {
            if(m_fExpanded)
                Expand(false);
            else
                Expand(true);
        }

        // Overrideables
        void DoPaint(CDCHandle dc)
        {
            // draw border
            RECT rcClient;
            GetClientRect(&rcClient);
            dc.FillRect(&rcClient, ::GetSysColorBrush(COLOR_3DFACE));

            RECT rcBorder = rcClient;
            int cy = m_rcButton.bottom / 2 - ::GetSystemMetrics(SM_CYEDGE);
            rcBorder.top = rcClient.top + cy;

            if(m_fExpanded)
                dc.DrawEdge(&rcBorder, BDR_RAISEDINNER | BDR_SUNKENOUTER, BF_RECT);
            else
            {
                rcBorder.bottom = rcBorder.top + cy + 2 * ::GetSystemMetrics(SM_CYEDGE);
                dc.DrawEdge(&rcBorder, BDR_RAISEDINNER | BDR_SUNKENOUTER, BF_RECT);
            }
        }

        // Message map and handlers
        typedef CRolloutCtrlImpl< T, TBase, TWinTraits >        thisClass;
        BEGIN_MSG_MAP(thisClass)
            MESSAGE_HANDLER(WM_CREATE, OnCreate)
            MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
            MESSAGE_HANDLER(WM_PAINT, OnPaint)
            MESSAGE_HANDLER(WM_PRINTCLIENT, OnPaint)
            MESSAGE_HANDLER(WM_ENABLE, OnEnable)
            MESSAGE_HANDLER(WM_MOVE, OnMove)
            MESSAGE_HANDLER(RCM_TOGGLEEXPANDCOLLAPSE, OnToggleExpandCollapse)
            MESSAGE_HANDLER(RCM_EXPAND, OnExpand)
            MESSAGE_HANDLER(RCM_COLLAPSE, OnCollapse)
            MESSAGE_HANDLER(RCM_ISEXPANDED, OnIsExpanded)
            MESSAGE_HANDLER(RCM_GETRECT, OnGetRect)
            COMMAND_CODE_HANDLER(BN_CLICKED, OnButtonClicked)
            COMMAND_CODE_HANDLER(BN_SETFOCUS, OnButtonSetFocus)
            ALT_MSG_MAP(1)
            MESSAGE_HANDLER(WM_LBUTTONDOWN, ForwardToContainer)
            MESSAGE_HANDLER(WM_LBUTTONUP, ForwardToContainer)
            MESSAGE_HANDLER(WM_MOUSEMOVE, ForwardToContainer)
            MESSAGE_HANDLER(WM_CONTEXTMENU, ForwardToContainer)
            MESSAGE_HANDLER(WM_SETCURSOR, ForwardToContainer)
            END_MSG_MAP()

            LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
            {
                Init();
                bHandled = FALSE;
                return 1;
            }

        LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
        {
            return 1;   // no background needed
        }

        LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
        {
            T* pT = static_cast<T*>(this);
            if(wParam != NULL)
            {
                pT->DoPaint((HDC)wParam);
            }
            else
            {
                CPaintDC dc(m_hWnd);
                pT->DoPaint(dc.m_hDC);
            }
            return 0;
        }

        LRESULT OnEnable(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
        {
            m_fEnabled = wParam ? true : false;
            m_button.EnableWindow(m_fEnabled);
            if((m_dwExtendedStyle & RCS_COLLAPSEDISABLED) != 0)
            {
                if(m_fEnabled)
                    Expand(true);
                else
                    Expand(false);
                ::SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd);
            }
            else
                m_Child.EnableWindow(m_fEnabled);
            bHandled = FALSE;
            return 1;
        }

        LRESULT OnMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
        {
            // Update child pos
            m_Child.SetWindowPos(NULL, m_sizeBorder.cx, m_sizeBorder.cy + m_rcButton.bottom,
                                 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            bHandled = FALSE;
            return 1;
        }

        LRESULT OnToggleExpandCollapse(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
        {
            ToggleExpandCollapse();
            return 0;
        }

        LRESULT OnExpand(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
        {
            Expand(true);
            return 0;
        }

        LRESULT OnCollapse(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
        {
            Expand(false);
            return 0;
        }

        LRESULT OnIsExpanded(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
        {
            return IsExpanded();
        }

        LRESULT OnGetRect(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
        {
            GetRect((wParam != 0), (RECT*)lParam);
            return 0;
        }

        LRESULT OnButtonClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl, BOOL& bHandled)
        {
            if(hWndCtl == m_button.m_hWnd)
            {
                ToggleExpandCollapse();
                ::SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd);
            }
            bHandled = FALSE;
            return 1;
        }

        LRESULT OnButtonSetFocus(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl, BOOL& bHandled)
        {
            if(hWndCtl == m_button.m_hWnd)
                // Ensure that we are visible
                ::SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd);
            bHandled = FALSE;
            return 1;
        }

        LRESULT ForwardToContainer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
        {
            DWORD dwPos = GetMessagePos();
            POINT ptPos = { GET_X_LPARAM(dwPos), GET_Y_LPARAM(dwPos) };
            // Forward the message only if the cursor is not inside a control
            if(::WindowFromPoint(ptPos) == m_Child.m_hWnd)
            {
                if(uMsg == WM_SETCURSOR)
                    ::PostMessage(GetParent(), uMsg, (WPARAM) (HWND) GetParent (), lParam);
                else
                {
                    ::ScreenToClient(GetParent(), &ptPos);
                    ::PostMessage(GetParent(), uMsg, wParam, MAKELPARAM(ptPos.x, ptPos.y));
                }
            }
            bHandled = FALSE;
            return 1;
        }

        // Implementation
        void Init()
        {
            // We need this style to prevent Windows from painting the button
            ModifyStyle(0, /*WS_GROUP | WS_TABSTOP | */BS_OWNERDRAW);
            ModifyStyleEx(0, WS_EX_CONTROLPARENT);

            m_fEnabled = IsWindowEnabled() ? true : false;
            m_sizeBorder.cx = ::GetSystemMetrics(SM_CXEDGE);
            m_sizeBorder.cy = ::GetSystemMetrics(SM_CYEDGE);

            // Workaround for contained dialog...
            m_Child.TChild::Create(m_hWnd, (LPARAM)NULL);
            HWND hWnd = m_Child.m_hWnd;
            m_Child.m_hWnd = NULL;
            m_Child.SubclassWindow(hWnd);
            m_Child.ModifyStyleEx(0, WS_EX_CONTROLPARENT);

            m_Child.ShowWindow(SW_SHOWNORMAL);
            m_Child.GetWindowRect(&m_rcExpanded);
            ScreenToClient(&m_rcExpanded);

            HFONT hFont = m_Child.GetFont();//AtlGetDefaultGuiFont();
            SetFont(hFont);

            CWindowDC dc(m_hWnd);
            HFONT hOldFont = dc.SelectFont(hFont);
            SIZE ptTextExtent;
            TCHAR szTitle[MAX_PATH];
            GetWindowText(szTitle, MAX_PATH);
            dc.GetTextExtent(szTitle, -1, &ptTextExtent);
            SIZE ptTextExtent2;
            dc.GetTextExtent(_T ("+"), -1, &ptTextExtent2);
            if (ptTextExtent .cx < ptTextExtent2 .cx)
                ptTextExtent .cx = ptTextExtent2 .cx;
            if (ptTextExtent .cy < ptTextExtent2 .cy)
                ptTextExtent .cy = ptTextExtent2 .cy;
            dc.SelectFont(hOldFont);

            int cyButton = ptTextExtent.cy + 2 * m_sizeBorder.cy;

            ::InflateRect(&m_rcExpanded, m_sizeBorder.cx, m_sizeBorder.cy);
            m_rcExpanded.bottom += cyButton;

            m_rcCollapsed = m_rcExpanded;
            m_rcButton    = m_rcExpanded;
            m_rcCollapsed.bottom = m_rcCollapsed.top + cyButton;

            // Create button
            m_rcButton.top    += m_sizeBorder.cy;
            m_rcButton.left   += 2 * m_sizeBorder.cx;
            m_rcButton.right  -= 1 * m_sizeBorder.cx;
            m_rcButton.bottom = m_rcButton.top + cyButton;

            m_button.Create(m_hWnd, szTitle,
                            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_NOTIFY);
            m_button.SetFont(hFont);
            m_button.SetWindowPos(HWND_TOP, &m_rcButton, SWP_NOACTIVATE);
            m_button.SetChecked(true);

            SetWindowPos(NULL, &m_rcExpanded, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
        }
};

template <class TChild>
class CRolloutCtrl : public CRolloutCtrlImpl<CRolloutCtrl<TChild>, TChild>
{
    public:
        DECLARE_WND_SUPERCLASS(_T("WTL_RolloutCtrl"), GetWndClassName())

            CRolloutCtrl() : CRolloutCtrlImpl<CRolloutCtrl<TChild>, TChild>()
        { }
};

/////////////////////////////////////////////////////////////////////////////
// CRolloutContainerImpl - 

template <DWORD t_dwStyle, DWORD t_dwExStyle>
class CRolloutContainerImplTraits
{
    public:
        static DWORD GetWndStyle(DWORD dwStyle)
        {
            return dwStyle == 0 ? t_dwStyle : dwStyle;
        }
        static DWORD GetWndExStyle(DWORD dwExStyle)
        {
            return dwExStyle == 0 ? t_dwExStyle : dwExStyle;
        }
};

typedef CRolloutContainerImplTraits<WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, WS_EX_CLIENTEDGE | WS_EX_CONTROLPARENT>  CRolloutContainerTraits;

// Messages
#define RCCM_BASE                       (WM_USER + 2)
// Ensures that the given rect is visible
// WPARAM unused
// LPARAM ptr to rect in screen space
#define RCCM_ENSUREVISIBLE      (RCCM_BASE + 0)

template <class T, class TBase = CWindow, class TWinTraits = CRolloutContainerTraits>
class ATL_NO_VTABLE CRolloutContainerImpl : public CWindowImpl<T, TBase, TWinTraits>
{
    public:
        DECLARE_WND_CLASS_EX(NULL, 0, COLOR_3DFACE)

            // Data declarations and members
            CSimpleArray<HWND> m_aRolloutCtrls;
        SIZE m_sizePage;
        SIZE m_sizeCtrls;
        POINT m_ptDragStart;
        HCURSOR m_hCursorHand;
        int m_nSpacing;
        int m_nCtrlsOffsetY;
        RECT m_rcSlider;
#if !((_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400))
        UINT m_uMsgMouseWheel;          // MSH_MOUSEWHEEL
        // Note that this message must be forwarded from a top level window
#endif //!((_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400))

        // Construction
        CRolloutContainerImpl()
        {}

        HWND Create(HWND hWndParent, LPCTSTR lpstrTitle = NULL, DWORD dwStyle = 0,
                    DWORD dwExStyle = 0, UINT nID = 0, LPVOID lpCreateParam = NULL)
        {
            return CWindowImpl< T, TBase, TWinTraits >::Create(hWndParent, rcDefault, lpstrTitle,
                                                               TWinTraits::GetWndStyle(dwStyle), TWinTraits::GetWndExStyle(dwExStyle), nID, lpCreateParam);
        }
        HWND Create(HWND hWndParent, UINT uTitleID, DWORD dwStyle = 0,
                    DWORD dwExStyle = 0, UINT nID = 0, LPVOID lpCreateParam = NULL)
        {
            TCHAR szTitle[MAX_PATH];
            if(uTitleID != 0U)
                ::LoadString(_Module.GetResourceInstance(), uTitleID, szTitle, sizeof(szTitle));
            return CWindowImpl< T, TBase, TWinTraits >::Create(hWndParent, rcDefault, szTitle,
                                                               TWinTraits::GetWndStyle(dwStyle), TWinTraits::GetWndExStyle(dwExStyle), nID, lpCreateParam);
        }

        // Attributes

        // Operations
        void GetClientSize(SIZE* pClientSize)
        {
            pClientSize->cx = m_sizeCtrls.cx + 5 * ::GetSystemMetrics(SM_CXEDGE);
            pClientSize->cy = m_sizeCtrls.cy;
        }
        int GetSpacing()
        {
            return m_nSpacing;
        }
        void SetSpacing(int nSpacing)
        {
            int d = nSpacing - m_nSpacing;
            m_sizeCtrls.cx += 2 * d;
            m_sizeCtrls.cy += 2 * d;
            m_nSpacing = nSpacing;
            UpdateLayout();
        }

        int AddRollout(HWND hWndRollout)
        {
            RECT rc;
            ::GetWindowRect(hWndRollout, &rc);
            m_sizeCtrls.cx = max(m_sizeCtrls.cx, rc.right - rc.left + 2 * m_nSpacing);
            m_sizeCtrls.cy += rc.bottom - rc.top + m_nSpacing;
            bool res = ( m_aRolloutCtrls.Add(hWndRollout) == TRUE );
            if(res)
                UpdateLayout();
            return res ? m_aRolloutCtrls.GetSize() - 1 : -1;
        }

        bool RemoveRollout(HWND hWndRollout)
        {
            RECT rc;
            ::GetWindowRect(hWndRollout, &rc);
            m_sizeCtrls.cy -= rc.bottom - rc.top;
            bool res = ( m_aRolloutCtrls.Remove(hWndRollout) == TRUE );
            if(res)
                UpdateLayout();
            return res;
        }
        bool RemoveRollout(int nIndex)
        {
            HWND hWndRollout = m_aRolloutCtrls[nIndex];
            return RemovePanel(hWndRollout);
        }
        bool RemoveAllRollouts()
        {
            bool res = ( m_aRolloutCtrls.RemoveAll() == TRUE );
            if(res)
                UpdateLayout();
            return res;
        }

        int GetRolloutCount()
        {
            return m_aRolloutCtrls.GetSize();
        }
        HWND GetRollout(int nIndex)
        {
            return m_aRolloutCtrls[nIndex];
        }

        void ExpandRollout(HWND hWndRollout, bool fExpand = true, bool fUpdate = true)
        {
            if(fExpand)
                ::SendMessage(hWndRollout, RCM_EXPAND, 0, 0);
            else
                ::SendMessage(hWndRollout, RCM_COLLAPSE, 0, 0);
            if(fUpdate)
                UpdateLayout();
        }
        void ExpandRollout(int nIndex, bool fExpand = true, bool fUpdate = true)
        {
            ExpandRollout(m_aRolloutCtrls[nIndex], fExpand, fUpdate);
        }
        void ExpandAllRollouts(bool fExpand = true)
        {
            for(int i = 0; i < m_aRolloutCtrls.GetSize(); i++)
                ExpandRollout(m_aRolloutCtrls[i], fExpand, false);
            UpdateLayout();
        }

        bool IsRolloutExpanded(HWND hWndRollout)
        {
            return ( ::SendMessage(hWndRollout, RCM_ISEXPANDED, 0, 0) != 0 );
        }
        bool IsRolloutExpanded(int nIndex)
        {
            return IsRolloutExpanded(m_aRolloutCtrls[nIndex]);
        }

        void EnableRollout(HWND hWndRollout, bool fEnable)
        {
            ::EnableWindow(hWndRollout, fEnable);
        }
        void EnableRollout(int nIndex, bool fEnable)
        {
            EnableRollout(m_aRolloutCtrls[nIndex], fEnable);
        }

        bool IsRolloutEnabled(HWND hWndRollout)
        {
            return ( ::IsWindowEnabled(hWndRollout) == TRUE );
        }
        bool IsRolloutEnabled(int nIndex)
        {
            return IsRolloutEnabled(m_aRolloutCtrls[nIndex]);
        }

        void ScrollToRollout(HWND hWndRollout)
        {
            RECT rc;
            ::GetWindowRect(hWndRollout, &rc);
            ::SendMessage(hWndRollout, RCCM_ENSUREVISIBLE, 0, (LPARAM)&rc);
        }
        void ScrollToRollout(int nIndex)
        {
            HWND hWndRollout = m_aRolloutCtrls[nIndex];
            ScrollToRollout(hWndRollout);
        }

        // Overrideables
        void DoPaint(CDCHandle dc)
        {
            //
            // draw slider
            //

            // clear background
            RECT rcSlider;
            GetClientRect(&rcSlider);
            dc.FillRect(&rcSlider, ::GetSysColorBrush(COLOR_3DFACE));

            // border
            ::InflateRect(&rcSlider, -1 * ::GetSystemMetrics(SM_CXEDGE), -1 * ::GetSystemMetrics(SM_CYEDGE));
            rcSlider.left = rcSlider.right - 3 * ::GetSystemMetrics(SM_CXEDGE);
            dc.DrawEdge(&rcSlider, BDR_SUNKENOUTER, BF_RECT);

            // pos
            ::CopyRect(&rcSlider, &m_rcSlider);
            ::InflateRect(&rcSlider, -1 * ::GetSystemMetrics(SM_CXEDGE), -1 * ::GetSystemMetrics(SM_CYEDGE));

            dc.FillRect(&rcSlider, ::GetSysColorBrush(COLOR_ACTIVECAPTION));
        }
        void InvalidateSlider()
        {
            RECT rcSlider;
            GetClientRect(&rcSlider);
            ::InflateRect(&rcSlider, -1 * ::GetSystemMetrics(SM_CXEDGE), -1 * ::GetSystemMetrics(SM_CYEDGE));
            rcSlider.left = rcSlider.right - 3 * ::GetSystemMetrics(SM_CXEDGE);
            InvalidateRect(&rcSlider, FALSE);
            UpdateWindow();
        }
        BOOL PreTranslateMessage(MSG* pMsg)
        {
            return ::IsDialogMessage(m_hWnd, pMsg);
        }

        // Message map and handlers
        typedef CRolloutContainerImpl< T, TBase, TWinTraits > thisClass;
        BEGIN_MSG_MAP(thisClass)
            MESSAGE_HANDLER(WM_SIZE, OnSize)
            MESSAGE_HANDLER(WM_PAINT, OnPaint)
            MESSAGE_HANDLER(WM_PRINTCLIENT, OnPaint)
            MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
            MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
            MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
            MESSAGE_HANDLER(WM_MOUSEWHEEL, OnMouseWheel)
#if !((_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400))
            MESSAGE_HANDLER(m_uMsgMouseWheel, OnMouseWheel)
#endif //(_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
            MESSAGE_HANDLER(WM_CONTEXTMENU, OnContextMenu)
            MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
            MESSAGE_HANDLER(DM_GETDEFID, OnGetDefId)
            MESSAGE_HANDLER(RCCM_ENSUREVISIBLE, OnEnsureVisible)
            COMMAND_CODE_HANDLER(BN_CLICKED, OnRolloutCtrlClicked)
            END_MSG_MAP()

            LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
            {
                m_sizePage.cx = LOWORD(lParam);
                m_sizePage.cy = HIWORD(lParam);

                if((m_sizePage.cy + m_nCtrlsOffsetY) > m_sizeCtrls.cy )
                    m_nCtrlsOffsetY = m_sizeCtrls.cy - m_sizePage.cy;

                UpdateSliderRect();
                InvalidateSlider();

                UpdateLayout();

                bHandled = FALSE;
                return 1;
            }

        LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
        {
            if(wParam != NULL)
            {
                DoPaint((HDC)wParam);
            }
            else
            {
                CPaintDC dc(m_hWnd);
                DoPaint(dc.m_hDC);
            }
            bHandled = FALSE;
            return 1;
        }

        LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
        {
            SetCapture();
            m_ptDragStart.x = GET_X_LPARAM(lParam);
            m_ptDragStart.y = GET_Y_LPARAM(lParam);

            RECT rect;
            GetClientRect(&rect);
            rect.left = m_rcSlider.left;
            rect.right = m_rcSlider.right;
            if(!::PtInRect(&m_rcSlider, m_ptDragStart) && ::PtInRect(&rect, m_ptDragStart))
            {
                int nNewOffsetY = -1;
                if(m_ptDragStart.y < m_rcSlider.top)
                    nNewOffsetY = m_ptDragStart.y;
                else if(m_ptDragStart.y > m_rcSlider.bottom)
                    nNewOffsetY = m_ptDragStart.y - (m_rcSlider.bottom - m_rcSlider.top);
                if(nNewOffsetY != 0)
                {
                    int nPageDeltaY = m_sizeCtrls.cy - m_sizePage.cy;
                    int nOldOffset = m_nCtrlsOffsetY;
                    float fProp = (float)m_sizeCtrls.cy / (float)m_sizePage.cy;
                    m_nCtrlsOffsetY = (int)((float)nNewOffsetY * fProp);
                    if(m_nCtrlsOffsetY < 0)
                        m_nCtrlsOffsetY = 0;
                    else if(m_nCtrlsOffsetY > nPageDeltaY)
                        m_nCtrlsOffsetY = nPageDeltaY;
                    if(nOldOffset != m_nCtrlsOffsetY)
                        UpdateLayout();
                }
            }
            return 0;
        }

        LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
        {
            ::ReleaseCapture();
            return 0;
        }

        LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
        {
            if(::GetCapture() == m_hWnd && (wParam & MK_LBUTTON))
            {
                int nPageDeltaY = m_sizeCtrls.cy - m_sizePage.cy;
                if(nPageDeltaY > 0)
                {
                    int nDeltaY = (GET_Y_LPARAM(lParam) - m_ptDragStart.y);
                    if(nDeltaY != 0)
                    {
                        if(/*GET_X_LPARAM(lParam) < m_sizeCtrls.cx && */m_ptDragStart.x < m_sizeCtrls.cx)
                            nDeltaY *= -1;

                        int nOldOffset = m_nCtrlsOffsetY;
                        float fProp = (float)m_sizeCtrls.cy / (float)m_sizePage.cy;
                        m_nCtrlsOffsetY += (int)((float)nDeltaY * fProp);
                        if(m_nCtrlsOffsetY < 0)
                            m_nCtrlsOffsetY = 0;
                        else if(m_nCtrlsOffsetY > nPageDeltaY)
                            m_nCtrlsOffsetY = nPageDeltaY;
                        //                                      m_ptDragStart.x = GET_X_LPARAM(lParam);
                        m_ptDragStart.y = GET_Y_LPARAM(lParam);
                        if(nOldOffset != m_nCtrlsOffsetY)
                            UpdateLayout();
                    }
                }
                else
                    m_nCtrlsOffsetY = 0;
            }
            bHandled = FALSE;
            return 1;
        }

        LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
        {
            T* pT = static_cast<T*>(this);
            ATLASSERT(::IsWindow(pT->m_hWnd));

#if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
            uMsg;
            int zDelta = (int)(short)HIWORD(wParam);
#else
            int zDelta = (uMsg == WM_MOUSEWHEEL) ? (int)(short)HIWORD(wParam) : (int)wParam;
#endif //!((_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400))

            int nPageDeltaY = m_sizeCtrls.cy - m_sizePage.cy;
            if(nPageDeltaY > 0)
            {
                int nDeltaY = -zDelta/4;
                if(nDeltaY != 0)
                {
                    int nOldOffset = m_nCtrlsOffsetY;
                    float fProp = (float)m_sizeCtrls.cy / (float)m_sizePage.cy;
                    //                          m_nCtrlsOffsetY += nDeltaY;
                    m_nCtrlsOffsetY += (int)((float)nDeltaY * fProp);
                    if(m_nCtrlsOffsetY < 0)
                        m_nCtrlsOffsetY = 0;
                    else if(m_nCtrlsOffsetY > nPageDeltaY)
                        m_nCtrlsOffsetY = nPageDeltaY;
                    if(nOldOffset != m_nCtrlsOffsetY)
                        UpdateLayout();
                }
            }
            else
                m_nCtrlsOffsetY = 0;

            return 0;
        }

        LRESULT OnContextMenu(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
        {
            POINT ptPos = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            ScreenToClient(&ptPos);
            ::SetCursor(::LoadCursor(NULL, IDC_ARROW));

            HWND hWndCurRollOut = NULL;
            HMENU hMenu = ::CreatePopupMenu();
            ::AppendMenu(hMenu, MF_STRING | MF_GRAYED, 1, _T("Close Rollout"));
            ::AppendMenu(hMenu, MF_STRING, 2, _T("Close All"));
            ::AppendMenu(hMenu, MF_STRING, 3, _T("Open All"));
            ::AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
            for(int i = 0; i < m_aRolloutCtrls.GetSize(); i++)
            {
                UINT uFlags = MF_STRING;
                TCHAR szName[256];
                HWND hWndRollout = m_aRolloutCtrls[i];
                ::GetWindowText(hWndRollout, szName, 255);
                if(::SendMessage(hWndRollout, RCM_ISEXPANDED, 0, 0))
                    uFlags |= MF_CHECKED;
                if(::IsWindowEnabled(hWndRollout) == FALSE)
                    uFlags |= MF_GRAYED;
                RECT rc;
                ::GetWindowRect(hWndRollout, &rc);
                ScreenToClient(&rc);
                if(::PtInRect(&rc, ptPos) == TRUE)
                {
                    hWndCurRollOut = m_aRolloutCtrls[i];
                    ::EnableMenuItem(hMenu, 1, MF_BYCOMMAND | MF_ENABLED);
                }
                ::AppendMenu(hMenu, uFlags, i + 4, szName);
            }

            UINT uCmdID = (UINT)::TrackPopupMenu(hMenu,
                                                 TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD | TPM_NONOTIFY | TPM_RIGHTBUTTON,
                                                 GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 0, m_hWnd, NULL);
            switch(uCmdID)
            {
                case 0:
                    // User canceled
                    break;
                case 1:
                    // Close current
                    if(hWndCurRollOut != NULL)
                    {
                        ::SendMessage(hWndCurRollOut, RCM_TOGGLEEXPANDCOLLAPSE, 0, 0);
                        UpdateLayout();
                    }
                    break;
                case 2:
                    // Close all
                case 3:
                    // Open all
                    {
                        UINT uMsg = (uCmdID == 2) ? RCM_COLLAPSE : RCM_EXPAND;
                        for(int i = 0; i < m_aRolloutCtrls.GetSize(); i++)
                        {
                            ::SendMessage(m_aRolloutCtrls[i], uMsg, 0, 0);
                        }
                        UpdateLayout();
                    }
                    break;
                default:
                    // Toggle the control under the cursor
                    ::SendMessage(m_aRolloutCtrls[uCmdID - 4], RCM_TOGGLEEXPANDCOLLAPSE, 0, 0);
                    UpdateLayout();
                    break;
            }
            return 0;
        }

        LRESULT OnSetCursor(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
        {
            if((HWND)wParam == m_hWnd)
            {
                ::SetCursor(m_hCursorHand);
                return TRUE;
            }
            else
                bHandled = FALSE;
            return 0;
        }

        LRESULT OnGetDefId(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
        {
            return 0;
        }

        LRESULT OnEnsureVisible(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
        {
            RECT rect;
            ::CopyRect(&rect, (RECT*)lParam);
            ScreenToClient(&rect);
            if(rect.top < 0)
            {
                m_nCtrlsOffsetY += rect.top - m_nSpacing;
                UpdateLayout();
            }
            else if(rect.bottom > m_sizePage.cy)
            {
                m_nCtrlsOffsetY += rect.bottom + m_nSpacing - m_sizePage.cy;
                UpdateLayout();
            }
            return 0;
        }

        LRESULT OnRolloutCtrlClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl, BOOL& bHandled)
        {
            BOOL fDone = FALSE;

            T* pT = static_cast<T*>(this);
            ATLASSERT(::IsWindow(pT->m_hWnd));
            for(int i = 0; i < m_aRolloutCtrls.GetSize() && fDone == FALSE; i++)
            {
                if(m_aRolloutCtrls[i] == hWndCtl)
                {
                    // This rollout control has been clicked
                    fDone = TRUE;
                    UpdateLayout();

                    if(::SendMessage(hWndCtl, RCM_ISEXPANDED, 0, 0))
                    {
                        RECT rect;
                        ::GetWindowRect(hWndCtl, &rect);
                        SendMessage(RCCM_ENSUREVISIBLE, 0, (LPARAM)&rect);
                    }
                }
            }
            bHandled = FALSE;
            return 1;
        }

        // Implementation
        void RolloutContainer_Init()
        {
#if !((_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400))
            if(m_uMsgMouseWheel != 0)
                m_uMsgMouseWheel = ::RegisterWindowMessage(MSH_MOUSEWHEEL);
#endif //!((_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400))

            m_hCursorHand = ::LoadCursor(NULL, IDC_HAND);
            if(m_hCursorHand == NULL)
                m_hCursorHand = ::LoadCursor(NULL, IDC_SIZENS);

            // clear out everything else
            m_nSpacing = ::GetSystemMetrics(SM_CYDLGFRAME);
            m_sizePage.cx = 0;
            m_sizePage.cy = 0;
            m_sizeCtrls.cx = 0;
            m_sizeCtrls.cy = 2 * m_nSpacing;
            m_nCtrlsOffsetY = 0;
        }
        void UpdateSliderRect()
        {
            GetClientRect(&m_rcSlider);
            ::InflateRect(&m_rcSlider, -1 * ::GetSystemMetrics(SM_CXEDGE), -1 * ::GetSystemMetrics(SM_CYEDGE));
            m_rcSlider.left = m_rcSlider.right - 3 * ::GetSystemMetrics(SM_CXEDGE);
            if(m_sizePage.cy < m_sizeCtrls.cy)
            {
                float fProp = (float)m_sizePage.cy / (float)m_sizeCtrls.cy;
                m_rcSlider.bottom = (int)((float)m_rcSlider.bottom * fProp);
                m_rcSlider.top += (int)((float)m_nCtrlsOffsetY * fProp);
                m_rcSlider.bottom += (int)((float)m_nCtrlsOffsetY * fProp);
            }
            else
                ::InflateRect(&m_rcSlider, -1 * ::GetSystemMetrics(SM_CXEDGE), -1 * ::GetSystemMetrics(SM_CYEDGE));
        }
        void UpdateLayout()
        {
            T* pT = static_cast<T*>(this);
            ATLASSERT(::IsWindow(pT->m_hWnd));

            HDWP hdwp = ::BeginDeferWindowPos(pT->m_aRolloutCtrls.GetSize());
            RECT rectCtl;
            int y = pT->m_nSpacing - pT->m_nCtrlsOffsetY;
            pT->m_sizeCtrls.cy = pT->m_nSpacing;
            for(int i = 0; i < pT->m_aRolloutCtrls.GetSize(); i++)
            {
                HWND ctl = pT->m_aRolloutCtrls[i];
                ::GetWindowRect(ctl, &rectCtl);
                ::DeferWindowPos(hdwp, ctl, NULL, pT->m_nSpacing, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

                // Update next pos and total hight of rollout controls
                y += (rectCtl.bottom - rectCtl.top) + pT->m_nSpacing;
                pT->m_sizeCtrls.cy += (rectCtl.bottom - rectCtl.top) + pT->m_nSpacing;
            }
            ::EndDeferWindowPos(hdwp);

            pT->m_sizeCtrls.cy += pT->m_nSpacing;
            if(pT->m_nCtrlsOffsetY != 0 && pT->m_sizeCtrls.cy < pT->m_sizePage.cy)
            {
                // All controls fit into the visible area, so show them all
                pT->m_nCtrlsOffsetY = 0;
                UpdateLayout();
            }

            pT->UpdateSliderRect();
            pT->InvalidateSlider();
        }
};

class CRolloutContainerClient : public CRolloutContainerImpl<CRolloutContainerClient>
{
    public:
        DECLARE_WND_SUPERCLASS(_T("WTL_RolloutContainer"), GetWndClassName())

            CRolloutContainerClient()
            : CRolloutContainerImpl<CRolloutContainerClient>()
            { }

        typedef CRolloutContainerImpl<CRolloutContainerClient> baseClass;
        BEGIN_MSG_MAP(CRolloutContainerClient)
            MESSAGE_HANDLER(WM_CREATE, OnCreate)
            CHAIN_MSG_MAP(baseClass)
            END_MSG_MAP()

            LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
            {
                RolloutContainer_Init();
                return 0;
            }
};

#endif // !defined(__ROLLOUTCTRL_H)

/* vim: set sw=4: */
