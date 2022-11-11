
struct id_name
{
	ULONG id;
	const char *name;
};

extern struct id_name reg_names[];

#define last_reg_id 0x1FC
#define max_table_size ((last_reg_id>>1)+1)

extern const char *id_anmes[max_table_size];

extern void init_qfind_lookup();

