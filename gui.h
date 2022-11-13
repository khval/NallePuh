
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
	struct Window* win_ptr;
	ULONG code;
	struct LogData log_data;
	struct PUHData* pd;
};

struct rc  ShowGUI( struct PUHData* pd );
struct rc HandleGUI( struct Window * window, struct PUHData* pd );
BOOL InstallPUH( ULONG  flags, ULONG audio_mode, ULONG frequency );


