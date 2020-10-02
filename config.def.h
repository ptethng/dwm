/* See LICENSE file for copyright and license details. */
#define TERMINAL "st"

/* appearance */
static unsigned int borderpx  = 1;        /* border pixel of windows */
static unsigned int snap      = 32;       /* snap pixel */
static const unsigned int gappih    = 10;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 10;       /* vert inner gap between windows */
static const unsigned int gappoh    = 10;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 10;       /* vert outer gap between windows and screen edge */
static const int smartgaps          = 1;        /* 1 means no outer gap when there is only one window */
static const int swallowfloating    = 1;        /* 1 means swallow floating windows by default */
static int showbar            = 1;        /* 0 means no bar */
static int topbar             = 1;        /* 0 means bottom bar */
static int hpadding           = 10;
static const char *fonts[]          = { 
	"Liberation Mono:pixelsize=16:antialias=true:autohint=true", 
	"JoyPixels:pixelsize=10:antialias=true:autohint=true",
	"Ubuntu Mono Nerd Font:size=8"
};
static const char dmenufont[]       = "Liberation Mono:size=10";
static char normbgcolor[]           = "#222222";
static char normbordercolor[]       = "#444444";
static char normfgcolor[]           = "#bbbbbb";
static char selfgcolor[]            = "#eeeeee";
static char selbordercolor[]        = "#005577";
static char selbgcolor[]            = "#005577";
static const unsigned int baralpha = OPAQUE;
static const unsigned int borderalpha = OPAQUE;
static char *colors[][3] = {
       /*               fg           bg           border   */
       [SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
       [SchemeSel]  = { selfgcolor,  selbgcolor,  selbordercolor  },
};
static const unsigned int alphas[][3]      = {
	/*               fg      bg        border     */
	[SchemeNorm] = { OPAQUE, baralpha, borderalpha },
	[SchemeSel]  = { OPAQUE, baralpha, borderalpha },
};

typedef struct {
	const char *name;
	const void *cmd;
} Sp;
const char *spcmd1[] = {"st", "-n", "spterm", "-g", "120x34", NULL };
const char *spcmd2[] = {"st", "-n", "spfm", "-g", "120x34", "-e", "vifmrun", NULL };
const char *spcmd3[] = {"st", "-n", "spaudio", "-g", "120x34", "-e", "pulsemixer", NULL };
static Sp scratchpads[] = {
	/* name          cmd  */
	{"spterm",      spcmd1},
	{"spfm",    spcmd2},
	{"spaudio",   spcmd3},
};

/* tagging */
static const char *tags[] = { "", "", "", "", "", "", "", "", "" };
static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      	instance    title       tags mask	isfloating	isterminal	noswallow  	monitor */
	{ "Firefox",  	NULL,		NULL,		1 << 8,		0,	0,			0,		-1 },
	{ "keepassxc",  NULL,		NULL,		1 << 8,		0,	0,			-1,		-1 },
	{ "Dragon-drag-and-drop",  NULL,		NULL,		0,		1,	0,			1,		-1 },
	{ "St",		NULL,		NULL,		0,		0,	1,			0,		-1 },
	{ "St",		NULL,		"newsboat",	0,		0,	1,			0,		-1 },
	{ NULL,		"spterm",	NULL,		SPTAG(0),	1,	1,			0,		-1 },
	{ NULL,		"spfm",		NULL,		SPTAG(1),	1,	1,			0,		-1 },
	{ NULL,		"spaudio",	NULL,		SPTAG(2),	1,	1,			0,		-1 },
};

/* layout(s) */
static float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static int nmaster     = 1;    /* number of clients in master area */
static int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "[M]",      monocle },
 	{ "[@]",      spiral },
 	{ "[\\]",      dwindle },
	{ "|M|",      centeredmaster },
	{ ">M>",      centeredfloatingmaster },
  { "TTT",      bstack },
	{ "===",      bstackhoriz },
	{ "><>",      NULL },    /* no layout function means floating behavior */
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },
#define STACKKEYS(MOD,ACTION) \
	{ MOD, XK_j,     ACTION##stack, {.i = INC(+1) } }, \
	{ MOD, XK_k,     ACTION##stack, {.i = INC(-1) } }, \
	{ MOD, XK_v,     ACTION##stack, {.i = 0 } }, \

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbgcolor, "-sf", selfgcolor, NULL };
static const char *termcmd[]  = { "st", NULL };
static const char *wallpaper[] = {"wallpaper-select", NULL};
static const char *emoji[] = {"emoji-select", NULL};
static const char *passmenu[] = {"passmenu", NULL};
static const char *browser[] = {"brave", NULL};
static const char *i3lock[] = {"i3lock-script", NULL};
 
/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
		{ "color0",             STRING,  &normbgcolor },
		{ "color8",	        STRING,  &normbordercolor },
		{ "color7",             STRING,  &normfgcolor },
		{ "color8",             STRING,  &selbgcolor },
		{ "color15",            STRING,  &selbordercolor },
		{ "color7",             STRING,  &selfgcolor },
		{ "borderpx",          	INTEGER, &borderpx },
		{ "snap",         	INTEGER, &snap },
		{ "showbar",          	INTEGER, &showbar },
		{ "topbar",          	INTEGER, &topbar },
		{ "nmaster",          	INTEGER, &nmaster },
		{ "resizehints",       	INTEGER, &resizehints },
		{ "mfact",      	FLOAT,   &mfact },
};

static Key keys[] = {
	/* modifier                     key        		function        argument */
	{ MODKEY,                       XK_d,      		spawn,          {.v = dmenucmd } },
	{ MODKEY,		        XK_Return, 		spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_p,      		spawn,          {.v = wallpaper } },
	{ MODKEY,                       XK_q,      		spawn,          {.v = passmenu } },
	{ MODKEY,                       XK_bracketleft,      	spawn,          {.v = emoji } },
	{ MODKEY,                       XK_b,      		spawn,      	{.v = browser} },
	{ MODKEY,                       XK_n,      		spawn,      	SHCMD("st -e newsboat") },
	{ MODKEY,                       XK_m,      		spawn,      	SHCMD("st -e neomutt") },
	{ MODKEY,            		XK_e,  	   		togglescratch,  {.ui = 0 } },
	{ MODKEY,            		XK_w,	   		togglescratch,  {.ui = 1 } },
	{ MODKEY,            		XK_r,	   		togglescratch,  {.ui = 2 } },
	{ MODKEY,            		XK_r,	   		spawn,  SHCMD("pkill -RTMIN+10 dwmblocks") },

	STACKKEYS(MODKEY,                   			focus)
	STACKKEYS(MODKEY|ShiftMask,                		push)
	{ MODKEY,                       XK_i,      		incnmaster,     {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_i,      		incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      		setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      		setmfact,       {.f = +0.05} },

	{ MODKEY|Mod4Mask,              XK_minus,  		incrgaps,       {.i = +20 } },
	{ MODKEY|Mod4Mask,              XK_equal,  		incrgaps,       {.i = -20 } },
	{ MODKEY|Mod4Mask,              XK_a,      		togglegaps,     {0} },
	{ MODKEY|Mod4Mask|ShiftMask,    XK_a,     		defaultgaps,    {0} },

	{ MODKEY,                       XK_Return, 		zoom,           {0} },
	{ MODKEY,                       XK_Tab,    		view,           {0} },
	{ MODKEY|ShiftMask,             XK_q,      		killclient,     {0} },
	{ MODKEY|ShiftMask,             XK_b,      		togglebar,      {0} },

	{ MODKEY,                       XK_t,      		setlayout,      {.v = &layouts[0]} },
	{ MODKEY|ShiftMask,             XK_t,      		setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_y,      		setlayout,      {.v = &layouts[2]} },
	{ MODKEY|ShiftMask,             XK_y,      		setlayout,      {.v = &layouts[3]} },
	{ MODKEY,                       XK_u,      		setlayout,      {.v = &layouts[4]} },
	{ MODKEY|ShiftMask,             XK_u,      		setlayout,      {.v = &layouts[5]} },
	{ MODKEY,                       XK_g,      		setlayout,      {.v = &layouts[6]} },
	{ MODKEY|ShiftMask,             XK_g,      		setlayout,      {.v = &layouts[7]} },
	{ MODKEY,                       XK_s,      		setlayout,      {.v = &layouts[8]} },

	{ MODKEY,                       XK_space,  		setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  		togglefloating, {0} },
	{ MODKEY,                       XK_f,      		togglefullscr,  {0} },

	{ MODKEY,	 	        XK_F5,     		xrdb,           {.v = NULL } },
	{ MODKEY,                       XK_0,      		view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      		tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  		focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, 		focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  		tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, 		tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY,	                XK_x,      spawn,          {.v = i3lock} },
	{ MODKEY|ShiftMask,             XK_x,      quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button1,        sigdwmblocks,   {.i = 1} },
	{ ClkStatusText,        0,              Button2,        sigdwmblocks,   {.i = 2} },
	{ ClkStatusText,        0,              Button3,        sigdwmblocks,   {.i = 3} },
	{ ClkStatusText,        0,              Button4,        sigdwmblocks,   {.i = 4} },
	{ ClkStatusText,        0,              Button5,        sigdwmblocks,   {.i = 5} },
	{ ClkStatusText,        ShiftMask,              Button1,        sigdwmblocks,   {.i = 6} },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button1,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

