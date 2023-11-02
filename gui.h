
struct LogData
{
	struct Gadget* m_Gadget;
	struct Window* m_Window;
};

struct rc 
{
	BOOL rc ;
	BOOL quit ;
	BOOL messed_with_registers;
	ULONG audio_mode ;
	ULONG frequency ;
//	struct Window* win_ptr;
	ULONG code;
	struct LogData log_data;
	struct PUHData* pd;
	char AHI_name[256];
};

extern struct rc ShowGUI( struct PUHData* pd, struct rc *rc );
extern void HandleGUI( struct Window * window, struct PUHData* pd, struct rc *rc );
extern BOOL InstallPUH( ULONG  flags, ULONG audio_mode, ULONG frequency );
extern int req(const char *title,const  char *body,const char *buttons, ULONG image);
extern const char *_L_default(LONG num) ;
extern const char *_L_catalog(unsigned int num) ;
extern const char *(*_L)(unsigned int num) ;

