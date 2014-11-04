

struct bloc_s{
  unsigned int n_bloc;
  struct bloc_s *next;
};
struct super_s{
  unsigned int mbr_magic;
  unsigned int nb_serie;
  unsigned int bloc_inode_racine;
  unsigned char name[32];
  struct bloc_s *bloc;
};
