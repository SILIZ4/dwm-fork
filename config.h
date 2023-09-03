#include <X11/XF86keysym.h>
/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx  = 3;        /* border pixel of windows */
static const unsigned int gappx     = 6;        /* gaps between windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const Bool showbar           = True;     /* 0 means no bar */
static const Bool topbar            = True;     /* 0 means bottom bar */
static const char *fonts[]          = { "fontawesome-regular:size=11", "NotoSans:size=11" };
static const char dmenufont[]       = "monospace:size=10";

static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const char col_gray1[]       = "#222222";  // black
static const char col_gray2[]       = "#444444";  // dark gray
static const char col_gray3[]       = "#bbbbbb";  // pale gray
static const char col_gray4[]       = "#eeeeee";  // white
//static const char col_cyan[]        = "#005577";
static const char col_main[]        = "#f2c43a";  // bright yellow
static const char col_pale[]        = "#eadcae";  // light yellow
static const char col_constrast[]   = "#4ec0c2";  // light blue


static const char *colors[][3]      = {
	/*                 fg             bg           border   */
	[SchemeNorm]   = { col_pale,      col_gray1,   col_gray1},
	[SchemeSel]    = { col_gray1,     col_main,    col_main },
	[SchemeDistro] = { col_constrast, col_gray1,   col_gray1 }
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class        instance    title         tags mask     isfloating   monitor */
    { "matplotlib", NULL,       NULL,         0,            1,           -1  },
    { NULL,         NULL,       "PySplitter", 0,            1,           -1  },
    { "Soffice",    "soffice",  NULL,         0,            0,           0,  },
    { "Soffice",    "soffice",  "Presenting", 0,            0,           1,  }
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_main, "-sf", col_gray4, NULL };
static const char *roficmd[] = { "rofi", "-modi",  "run", "-show", "run", "-fake-transpacency" };
static const char *termcmd[] = { "xfce4-terminal", "--hide-menubar" };

static const char *fileexplorercmd[] = { "kitty", "-e", "vifm" };
static const char *volumectlcmd[] = { "pavucontrol" };

static const char *mutecmd[] = { "amixer", "-D", "pulse", "sset", "Master", "toggle", NULL };
static const char *volupcmd[] = { "amixer", "-D", "pulse", "sset", "Master", "5%+", NULL };
static const char *voldowncmd[] = { "amixer", "-D", "pulse", "sset", "Master", "5%-", NULL };


// Additional functions
#include "fullscreen.c"
#include "movestack.c"
void focus_term(const Arg *arg);
void focus_browser(const Arg *arg);
void transfer_clients(const Arg *arg);
static const char *browser_names[] = {"Brave"};

static Key keys[] = {
	// Default commands
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          { .v = dmenucmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	//{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	//{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	//{ MODKEY,                       XK_z,      zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_q,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	//{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_s,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },

	// Additional commands
	{ MODKEY,                       XK_Return, spawn,          { .v = termcmd } },
	{ MODKEY|ControlMask,           XK_Return, focus_term,     {0} },
	{ MODKEY,                       XK_n,      spawn,          SHCMD("start-browser") },
	{ MODKEY|ControlMask,           XK_n,      focus_browser,  {0} },
	{ MODKEY,                       XK_d,      spawn,          { .v = roficmd } },
	{ MODKEY,                       XK_e,      spawn,          { .v = fileexplorercmd } },
	{ MODKEY,                       XK_v,      spawn,          { .v = volumectlcmd } },
	{ MODKEY,                       XK_c,      transfer_clients,{0} },
	{ MODKEY|ShiftMask,             XK_j,      movestack,      {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_k,      movestack,      {.i = -1 } },
	{ MODKEY,                       XK_f,      fullscreen,     {0} },
	{ 0,                            XF86XK_AudioMute, spawn,          { .v = mutecmd } },
	{ 0,                            XF86XK_AudioLowerVolume, spawn,   { .v = voldowncmd } },
	{ 0,                            XF86XK_AudioRaiseVolume, spawn,   { .v = volupcmd } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_BackSpace,      quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
