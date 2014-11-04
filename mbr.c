#include <stdio.h>
#include <stdlib.h>
#include "include/hardware.h"
#include "drive.h"
#include "mbr.h"

#define MBR_MAGIC 0xCAFE

struct mbr_s mbr;





/* int main(){ */

/*   initialisation(); */
/*   return 0; */
/* } */



int read_mbr(){
  read_sector_t(0,0,((unsigned char*) &(mbr)),sizeof(struct mbr_s));
  if(mbr.mbr_magic == MBR_MAGIC)
    return 0;
  mbr.nb_vol = 0;
  mbr.mbr_magic = MBR_MAGIC;
  return 1;
}

int save_mbr(){
  write_sector(0,0,(unsigned char*) &mbr);
  return 0;
}

unsigned int first_free_vol(){
  int i;
  i=0;
  while(1){
    if(!(mbr.vols[i].nb_sector))
      break;
    i++;
  }
    return i;
}
unsigned int new_vol(unsigned int nbloc){

  unsigned char buffer[HDA_SECTORSIZE];
  unsigned int first;
  for(first=0;first<HDA_SECTORSIZE;first++)
    buffer[first]=0;
  first = first_free_vol();
  write_bloc(first,nbloc,buffer);
  return first;

}

unsigned int blocToCyl(int vol,int nbloc){
  unsigned int cyl,sec;
  if(!vol || vol > mbr.nb_vol){
    fprintf(stderr,"Mauvais numero de volume");
    exit(EXIT_FAILURE);
  }
  if(!nbloc || nbloc > mbr.vols[vol].nb_sector){
    fprintf(stderr,"Mauvais numero de volume");
    exit(EXIT_FAILURE);
  }

  cyl = mbr.vols[vol].first_cyl;
  sec = mbr.vols[vol].first_sector;
  return (cyl+(sec+nbloc)/HDA_MAXSECTOR);
}

unsigned int blocToSec(int vol,int nbloc){
  unsigned int sec;
  if(!vol || vol > mbr.nb_vol){
    fprintf(stderr,"Mauvais numero de volume");
    exit(EXIT_FAILURE);
  }
  if(!nbloc || nbloc > mbr.vols[vol].nb_sector){
    fprintf(stderr,"Mauvais numero de volume");
    exit(EXIT_FAILURE);
  }

  sec = mbr.vols[vol].first_sector;
  return ((sec+nbloc)/HDA_MAXSECTOR);
}


void read_bloc(unsigned int vol, unsigned int nbloc,unsigned char *buffer){

  unsigned int cyl,sec;

  cyl = blocToCyl(vol,nbloc);
  sec = blocToSec(vol,nbloc);
  read_sector(cyl,sec,buffer);

}



void write_bloc(unsigned int vol, unsigned int nbloc,unsigned char *buffer){
  unsigned int cyl,sec;

  cyl = blocToCyl(vol,nbloc);
  sec = blocToSec(vol,nbloc);
  write_sector(cyl,sec,buffer);
}



void format_vol(unsigned int vol){
  unsigned int nbloc,cyl,sec;

  nbloc = mbr.vols[vol].nb_sector;
  cyl = mbr.vols[vol].first_cyl;
  sec = mbr.vols[vol].first_sector;
  format_sector(cyl,sec,nbloc,0);
}


void printType(enum vol_type type){
  switch (type){
  case BASE:
    printf("\nType de base\n");
    break;
  case ANNEX:
    printf("\nType annexe\n");
    break;
  default:
    printf("\nAutre types\n");
  }
}



void printf_vol(unsigned int i){
  printf("Volume %d:\n",i);
  printf("\tsize: %d\n",mbr.vols[i].nb_sector);
  printf("\t(fc, fs):(%d,%d)\n",mbr.vols[i].first_cyl,mbr.vols[i].first_sector);
  printType(mbr.vols[i].type);
  printf("\t(lc,ls); (%d,%d)\n\n",blocToCyl(i, mbr.vols[i].nb_sector),blocToSec(i,mbr.vols[i].nb_sector-1));
}

void listVolumes(){
  int i;
  for ( i = 0 ; i < mbr.nb_vol; i++){
    printf_vol(i);
  }
}


/* exit if the size is wrong */
void check_sector_size(){
	int real_value;
	_out(HDA_CMDREG, CMD_DSKINFO);
	real_value = (_in(HDA_DATAREGS+4)<<8)|_in(HDA_DATAREGS+5);
	if(real_value != HDA_SECTORSIZE){
		fprintf(stderr, "Error in sectors size\n\tsize found: %d\n\tsize expected: %d\n", real_value,HDA_SECTORSIZE);
		exit(EXIT_FAILURE);
	}
}


void initialisation(){
	int i;
	if(init_hardware("hardware.ini") == 0) {
		fprintf(stderr, "Error in hardware initialization\n");
		exit(EXIT_FAILURE);
	}

	check_sector_size();

	for(i=0; i<16; i++) IRQVECTOR[i] = empty_it;

	_mask(1);

	read_mbr();
	listVolumes();
}
