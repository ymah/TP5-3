#define MAX_VOL 8

enum vol_type {BASE,ANNEX,OTHER};

struct vol_descr{
  unsigned int first_cyl;
  unsigned int first_sector;
  unsigned int nb_sector;
  enum vol_type type;

};

typedef struct mbr_s *mbr_t;

struct mbr_s {

  unsigned int mbr_magic;
  unsigned int nb_vol;
  struct vol_descr vols[MAX_VOL];
};

int load_mbr();
int read_mbr();
int save_mbr();

unsigned int first_free_vol();
unsigned int new_vol();
unsigned int blocToCyl(int vol,int nbloc);
unsigned int blocToSec(int vol,int nbloc);

void read_bloc(unsigned int vol, unsigned int nbloc,unsigned char *buffer);
void write_bloc(unsigned int vol, unsigned int nbloc,unsigned char *buffer);
void format_vol(unsigned int vol);
void printType(enum vol_type type);
void listVolumes();
void printf_vol(unsigned int i);
void check_sector_size();
void initialisation();

void init_super(unsigned int vol);
int load_super(unsigned int vol);
void save_super();
unsigned int new_bloc();
void free_bloc(unsigned int bloc);

