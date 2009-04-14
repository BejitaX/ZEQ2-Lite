/*=======================================================================
GAME OPTIONS MENU
=======================================================================*/
#include "ui_local.h"
#define ART_FRAMEL				"menu/art/frame2_l"
#define ART_FRAMER				"menu/art/frame1_r"
#define ART_BACK0				"menu/art/back_0"
#define ART_BACK1				"menu/art/back_1"
#define PREFERENCES_X_POS		360
#define ID_CROSSHAIR			127
#define ID_CAMERASTYLE			128
#define	ID_BEAMCONTROL			129
#define	ID_BEAMDETAIL			130
#define ID_WALLMARKS			131
#define ID_DYNAMICLIGHTS		132
#define ID_IDENTIFYTARGET		133
#define ID_SYNCEVERYFRAME		134
#define	ID_MOTIONBLUR			135
#define ID_USERUNANIMATION		136
#define ID_ALLOWDOWNLOAD		137
#define ID_BACK					138
#define	NUM_CROSSHAIRS			10
typedef struct{
	menuframework_s		menu;
	menutext_s			banner;
	menubitmap_s		framel;
	menubitmap_s		framer;
	menulist_s			crosshair;
	menulist_s			camerastyle;
	menulist_s			beamdetail;
	menuradiobutton_s	wallmarks;
	menuradiobutton_s	dynamiclights;
	menuradiobutton_s	identifytarget;
	menulist_s			beamcontrol;
	menuradiobutton_s	synceveryframe;
	menuradiobutton_s	motionblur;
	menuradiobutton_s	useRunAnimation;
	menuradiobutton_s	allowdownload;
	menubitmap_s		back;
	qhandle_t			crosshairShader[NUM_CROSSHAIRS];
} preferences_t;
static preferences_t s_preferences;
static const char *beamdetail_names[] = {"Low","Medium","High","Very High",	0};
static const char *beamcontrol_names[] = {"Beam Head Focus","Crosshair Focus",0};
static const char *camerastyle_names[] = {"Locked Behind Head","Locked Behind Character",0};
static void Preferences_SetMenuItems( void ) {
	s_preferences.crosshair.curvalue		= (int)trap_Cvar_VariableValue( "cg_drawCrosshair" ) % NUM_CROSSHAIRS;
	s_preferences.camerastyle.curvalue		= Com_Clamp( 0, 1, trap_Cvar_VariableValue( "cg_thirdPersonCamera" ) );
	s_preferences.beamdetail.curvalue		= Com_Clamp( 0, 3, trap_Cvar_VariableValue( "r_beamDetail" ) );
	s_preferences.wallmarks.curvalue		= trap_Cvar_VariableValue( "cg_marks" ) != 0;
	s_preferences.identifytarget.curvalue	= trap_Cvar_VariableValue( "cg_drawCrosshairNames" ) != 0;
	s_preferences.dynamiclights.curvalue	= trap_Cvar_VariableValue( "r_dynamiclight" ) != 0;
	s_preferences.beamcontrol.curvalue		= Com_Clamp( 0, 1, trap_Cvar_VariableValue( "cg_beamControl" ) );
	s_preferences.synceveryframe.curvalue	= trap_Cvar_VariableValue( "r_finish" ) != 0;
	s_preferences.motionblur.curvalue		= trap_Cvar_VariableValue( "r_motionBlur" ) != 0;
	s_preferences.useRunAnimation.curvalue	= trap_Cvar_VariableValue( "g_running" ) != 0;
	s_preferences.allowdownload.curvalue	= trap_Cvar_VariableValue( "cl_allowDownload" ) != 0;
}
static void Preferences_Event( void* ptr, int notification ) {
	if(notification != QM_ACTIVATED){
		return;
	}
	switch(((menucommon_s*)ptr)->id){
	case ID_CROSSHAIR:
		s_preferences.crosshair.curvalue++;
		if( s_preferences.crosshair.curvalue == NUM_CROSSHAIRS ) {
			s_preferences.crosshair.curvalue = 0;
		}
		trap_Cvar_SetValue( "cg_drawCrosshair", s_preferences.crosshair.curvalue );
		break;
	case ID_CAMERASTYLE:
		trap_Cvar_SetValue( "cg_thirdPersonCamera", s_preferences.camerastyle.curvalue );
		break;
	case ID_BEAMCONTROL:
		trap_Cvar_SetValue( "cg_beamControl", s_preferences.beamcontrol.curvalue );
		break;
	case ID_BEAMDETAIL:
		trap_Cvar_SetValue( "r_beamDetail", s_preferences.beamdetail.curvalue * 10 + 10);
		break;
	case ID_WALLMARKS:
		trap_Cvar_SetValue( "cg_marks", s_preferences.wallmarks.curvalue );
		break;
	case ID_DYNAMICLIGHTS:
		trap_Cvar_SetValue( "r_dynamiclight", s_preferences.dynamiclights.curvalue );
		break;		
	case ID_IDENTIFYTARGET:
		trap_Cvar_SetValue( "cg_drawCrosshairNames", s_preferences.identifytarget.curvalue );
		break;
	case ID_SYNCEVERYFRAME:
		trap_Cvar_SetValue( "r_finish", s_preferences.synceveryframe.curvalue );
		break;
	case ID_MOTIONBLUR:
		trap_Cvar_SetValue( "r_motionBlur", s_preferences.motionblur.curvalue );
		break;
	case ID_USERUNANIMATION:
		trap_Cvar_SetValue( "g_running", s_preferences.useRunAnimation.curvalue );
		break;
	case ID_ALLOWDOWNLOAD:
		trap_Cvar_SetValue( "cl_allowDownload", s_preferences.allowdownload.curvalue );
		break;
	case ID_BACK:
		UI_PopMenu();
		break;
	}
}
/*=================
Crosshair_Draw
=================*/
static void Crosshair_Draw( void *self ) {
	menulist_s	*s;
	float		*color;
	int			x, y;
	int			style;
	qboolean	focus;
	s = (menulist_s *)self;
	x = s->generic.x;
	y =	s->generic.y;
	style = UI_SMALLFONT|UI_DROPSHADOW;
	focus = (s->generic.parent->cursor == s->generic.menuPosition);
	if(s->generic.flags & QMF_GRAYED){
		color = text_color_disabled;
	}
	else if(focus){
		color = text_color_highlight;
		style |= UI_PULSE;
	}
	else if(s->generic.flags & QMF_BLINK){
		color = text_color_highlight;
		style |= UI_BLINK;
	}
	else{
		color = text_color_normal;
	}
	if(focus){
		// draw cursor
		UI_FillRect( s->generic.left, s->generic.top, s->generic.right-s->generic.left+1, s->generic.bottom-s->generic.top+1, listbar_color ); 
		UI_DrawChar( x, y, 13, UI_CENTER|UI_BLINK|UI_SMALLFONT|UI_DROPSHADOW, color);
	}
	UI_DrawString( x - SMALLCHAR_WIDTH, y, s->generic.name, style|UI_RIGHT, color );
	if(!s->curvalue){return;}
	UI_DrawHandlePic( x + SMALLCHAR_WIDTH, y - 4, 24, 24, s_preferences.crosshairShader[s->curvalue] );
}
static void Preferences_MenuInit( void ) {
	int	y;
	memset( &s_preferences, 0 ,sizeof(preferences_t) );
	Preferences_Cache();
	s_preferences.menu.wrapAround = qtrue;
	s_preferences.menu.fullscreen = qtrue;
	s_preferences.banner.generic.type  = MTYPE_BTEXT;
	s_preferences.banner.generic.x	   = 320;
	s_preferences.banner.generic.y	   = 16;
	s_preferences.banner.string		   = "GAME OPTIONS";
	s_preferences.banner.color         = color_white;
	s_preferences.banner.style         = UI_CENTER|UI_DROPSHADOW;
	s_preferences.framel.generic.type  = MTYPE_BITMAP;
	s_preferences.framel.generic.name  = ART_FRAMEL;
	s_preferences.framel.generic.flags = QMF_INACTIVE;
	s_preferences.framel.generic.x	   = 0;
	s_preferences.framel.generic.y	   = 78;
	s_preferences.framel.width  	   = 256;
	s_preferences.framel.height  	   = 329;
	s_preferences.framer.generic.type  = MTYPE_BITMAP;
	s_preferences.framer.generic.name  = ART_FRAMER;
	s_preferences.framer.generic.flags = QMF_INACTIVE;
	s_preferences.framer.generic.x	   = 376;
	s_preferences.framer.generic.y	   = 76;
	s_preferences.framer.width  	   = 256;
	s_preferences.framer.height  	   = 334;

	y = 144;
	s_preferences.crosshair.generic.type		= MTYPE_TEXT;
	s_preferences.crosshair.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT|QMF_NODEFAULTINIT|QMF_OWNERDRAW;
	s_preferences.crosshair.generic.x			= PREFERENCES_X_POS;
	s_preferences.crosshair.generic.y			= y;
	s_preferences.crosshair.generic.name		= "Crosshair:";
	s_preferences.crosshair.generic.callback	= Preferences_Event;
	s_preferences.crosshair.generic.ownerdraw	= Crosshair_Draw;
	s_preferences.crosshair.generic.id			= ID_CROSSHAIR;
	s_preferences.crosshair.generic.top			= y - 4;
	s_preferences.crosshair.generic.bottom		= y + 20;
	s_preferences.crosshair.generic.left		= PREFERENCES_X_POS - ( ( strlen(s_preferences.crosshair.generic.name) + 1 ) * SMALLCHAR_WIDTH );
	s_preferences.crosshair.generic.right		= PREFERENCES_X_POS + 48;

	y += BIGCHAR_HEIGHT+2+4;
	s_preferences.camerastyle.generic.type        = MTYPE_SPINCONTROL;
	s_preferences.camerastyle.generic.name	      = "Flight Camera style:";
	s_preferences.camerastyle.generic.flags	      = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.camerastyle.generic.callback    = Preferences_Event;
	s_preferences.camerastyle.generic.id          = ID_CAMERASTYLE;
	s_preferences.camerastyle.generic.x	          = PREFERENCES_X_POS;
	s_preferences.camerastyle.generic.y	          = y;
	s_preferences.camerastyle.itemnames			  = camerastyle_names;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.beamcontrol.generic.type		= MTYPE_SPINCONTROL;
	s_preferences.beamcontrol.generic.name		= "Beam Control Style:";
	s_preferences.beamcontrol.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.beamcontrol.generic.callback	= Preferences_Event;
	s_preferences.beamcontrol.generic.id		= ID_BEAMCONTROL;
	s_preferences.beamcontrol.generic.x			= PREFERENCES_X_POS;
	s_preferences.beamcontrol.generic.y			= y;
	s_preferences.beamcontrol.itemnames			= beamcontrol_names;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.beamdetail.generic.type			  = MTYPE_SPINCONTROL;
	s_preferences.beamdetail.generic.name			  = "Beam Detail:";
	s_preferences.beamdetail.generic.flags	          = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.beamdetail.generic.callback         = Preferences_Event;
	s_preferences.beamdetail.generic.id               = ID_BEAMDETAIL;
	s_preferences.beamdetail.generic.x	              = PREFERENCES_X_POS;
	s_preferences.beamdetail.generic.y	              = y;
	s_preferences.beamdetail.itemnames				  = beamdetail_names;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.motionblur.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.motionblur.generic.name	  = "Motion Blur Effect:";
	s_preferences.motionblur.generic.flags	  = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.motionblur.generic.callback = Preferences_Event;
	s_preferences.motionblur.generic.id       = ID_MOTIONBLUR;
	s_preferences.motionblur.generic.x	      = PREFERENCES_X_POS;
	s_preferences.motionblur.generic.y	      = y;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.useRunAnimation.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.useRunAnimation.generic.name		= "Use Run Animations:";
	s_preferences.useRunAnimation.generic.flags	= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.useRunAnimation.generic.callback = Preferences_Event;
	s_preferences.useRunAnimation.generic.id       = ID_USERUNANIMATION;
	s_preferences.useRunAnimation.generic.x	       = PREFERENCES_X_POS;
	s_preferences.useRunAnimation.generic.y	       = y;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.dynamiclights.generic.type      = MTYPE_RADIOBUTTON;
	s_preferences.dynamiclights.generic.name	  = "Dynamic Lights:";
	s_preferences.dynamiclights.generic.flags     = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.dynamiclights.generic.callback  = Preferences_Event;
	s_preferences.dynamiclights.generic.id        = ID_DYNAMICLIGHTS;
	s_preferences.dynamiclights.generic.x	      = PREFERENCES_X_POS;
	s_preferences.dynamiclights.generic.y	      = y;

	y += BIGCHAR_HEIGHT;
	s_preferences.wallmarks.generic.type          = MTYPE_RADIOBUTTON;
	s_preferences.wallmarks.generic.name	      = "Marks on Walls:";
	s_preferences.wallmarks.generic.flags	      = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.wallmarks.generic.callback      = Preferences_Event;
	s_preferences.wallmarks.generic.id            = ID_WALLMARKS;
	s_preferences.wallmarks.generic.x	          = PREFERENCES_X_POS;
	s_preferences.wallmarks.generic.y	          = y;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.identifytarget.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.identifytarget.generic.name	  = "Identify Target:";
	s_preferences.identifytarget.generic.flags    = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.identifytarget.generic.callback = Preferences_Event;
	s_preferences.identifytarget.generic.id       = ID_IDENTIFYTARGET;
	s_preferences.identifytarget.generic.x	      = PREFERENCES_X_POS;
	s_preferences.identifytarget.generic.y	      = y;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.synceveryframe.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.synceveryframe.generic.name	  = "Sync Every Frame:";
	s_preferences.synceveryframe.generic.flags	  = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.synceveryframe.generic.callback = Preferences_Event;
	s_preferences.synceveryframe.generic.id       = ID_SYNCEVERYFRAME;
	s_preferences.synceveryframe.generic.x	      = PREFERENCES_X_POS;
	s_preferences.synceveryframe.generic.y	      = y;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.allowdownload.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.allowdownload.generic.name	   = "Automatic Downloading:";
	s_preferences.allowdownload.generic.flags	   = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.allowdownload.generic.callback = Preferences_Event;
	s_preferences.allowdownload.generic.id       = ID_ALLOWDOWNLOAD;
	s_preferences.allowdownload.generic.x	       = PREFERENCES_X_POS;
	s_preferences.allowdownload.generic.y	       = y;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.back.generic.type	    = MTYPE_BITMAP;
	s_preferences.back.generic.name     = ART_BACK0;
	s_preferences.back.generic.flags    = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_preferences.back.generic.callback = Preferences_Event;
	s_preferences.back.generic.id	    = ID_BACK;
	s_preferences.back.generic.x		= 0;
	s_preferences.back.generic.y		= 480-64;
	s_preferences.back.width  		    = 128;
	s_preferences.back.height  		    = 64;
	s_preferences.back.focuspic         = ART_BACK1;

	Menu_AddItem( &s_preferences.menu, &s_preferences.banner );
	Menu_AddItem( &s_preferences.menu, &s_preferences.framel );
	Menu_AddItem( &s_preferences.menu, &s_preferences.framer );
	Menu_AddItem( &s_preferences.menu, &s_preferences.crosshair );
	Menu_AddItem( &s_preferences.menu, &s_preferences.camerastyle );
	Menu_AddItem( &s_preferences.menu, &s_preferences.beamcontrol );
	Menu_AddItem( &s_preferences.menu, &s_preferences.beamdetail );
	Menu_AddItem( &s_preferences.menu, &s_preferences.motionblur );
	Menu_AddItem( &s_preferences.menu, &s_preferences.wallmarks );
	Menu_AddItem( &s_preferences.menu, &s_preferences.dynamiclights );
	Menu_AddItem( &s_preferences.menu, &s_preferences.identifytarget );
	Menu_AddItem( &s_preferences.menu, &s_preferences.synceveryframe );
	Menu_AddItem( &s_preferences.menu, &s_preferences.useRunAnimation );
	Menu_AddItem( &s_preferences.menu, &s_preferences.allowdownload );
	Menu_AddItem( &s_preferences.menu, &s_preferences.back );

	Preferences_SetMenuItems();
}
/*===============
Preferences_Cache
===============*/
void Preferences_Cache(void){
	int	n;
	trap_R_RegisterShaderNoMip(ART_FRAMEL);
	trap_R_RegisterShaderNoMip(ART_FRAMER);
	trap_R_RegisterShaderNoMip(ART_BACK0);
	trap_R_RegisterShaderNoMip(ART_BACK1);
	for(n=0;n<NUM_CROSSHAIRS;n++){
		s_preferences.crosshairShader[n] = trap_R_RegisterShaderNoMip( va("interface/hud/crosshair%c", 'a' + n ) );
	}
}
/*===============
UI_PreferencesMenu
===============*/
void UI_PreferencesMenu( void ) {
	Preferences_MenuInit();
	UI_PushMenu( &s_preferences.menu );
}
