#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/hardware.h"
#include "drive.h"
#include "mbr.h"

int verbose=0;
int test=0;

/* int main(int argc,char *argv[]) */
/* { */
/*   int i; */
/*   for(i=1;i<argc;i++){ */
/*     if(!strncmp(argv[i],"-t",3)){ */
/*       test=1; */
/*     }else if(!strncmp(argv[i],"-v",3)){ */
/*       verbose=1; */
/*     } */
/*   } */
/*   if(test) */
/*     programmeTest(); */
/*   return 0; */
/* } */

void dmps(int piste,int secteur){
  int i;
  for(i=0;i<16;i++)
    IRQVECTOR[i]= empty_it;


  /* chargement des donnée concernant la piste */
  _out(HDA_DATAREGS,piste>>8);
  _out(HDA_DATAREGS+1,piste & 0x00FF);

  /* chargement des donnée concernant le secteur  */
  _out(HDA_DATAREGS+2,secteur>>8);
  _out(HDA_DATAREGS+3,secteur & 0x00FF);

  /*commande pour positionner la tete de lecture */
  _out(HDA_CMDREG,CMD_SEEK);
  _sleep(HDA_IRQ);

  _out(HDA_DATAREGS,0);
  _out(HDA_DATAREGS+1,1);
  _out(HDA_CMDREG,CMD_READ);
  _sleep(HDA_IRQ);


  for(i=0;i<HDA_SECTORSIZE;i++)
    printf("%x ",MASTERBUFFER[i]);

  return ;
}


void read_sector_t(unsigned int piste, unsigned int secteur,unsigned char *buffer,unsigned int size){

  int i;
  deplacerTete(piste,secteur);
  _out(HDA_DATAREGS,0);
  _out(HDA_DATAREGS+1,1);
  _out(HDA_CMDREG,CMD_READ);
  _sleep(HDA_IRQ);

  for(i=0;i<size;i++)
    buffer[i] = MASTERBUFFER[i];
}

void read_sector(unsigned int piste, unsigned int secteur,unsigned char *buffer){

  read_sector_t(piste,secteur,buffer,HDA_SECTORSIZE);
}

void deplacerTete(unsigned int piste,unsigned int secteur){

  int i;
  if(piste < 0 || piste > HDA_SECTORSIZE){
    printf("numero de piste incorrect");
    exit(EXIT_FAILURE);
  }
  if(secteur < 0 || piste > HDA_MAXSECTOR){
    printf("numero de secteur incorrect");
    exit(EXIT_FAILURE);
  }

  for(i=0;i<16;i++)
    IRQVECTOR[i]= empty_it;

  /* chargement des donnée concernant la piste */
  _out(HDA_DATAREGS,piste>>8);
  _out(HDA_DATAREGS+1,piste & 0x00FF);

  /* chargement des donnée concernant le secteur  */
  _out(HDA_DATAREGS+2,secteur>>8);
  _out(HDA_DATAREGS+3,secteur & 0x00FF);
  _out(HDA_CMDREG,CMD_SEEK);

  _sleep(HDA_IRQ);
}




void write_sector_t(unsigned int piste,unsigned int secteur, unsigned char *buffer,unsigned int size){
  int i;
  deplacerTete(piste,secteur);
  for(i=0;i<size;i++)
    MASTERBUFFER[i]=buffer[i];
  _out(HDA_DATAREGS,0);
  _out(HDA_DATAREGS+1,1);
  _out(HDA_CMDREG,CMD_WRITE);
  _sleep(HDA_IRQ);
}

void write_sector(unsigned int piste,unsigned int secteur,unsigned char *buffer){

  write_sector_t(piste,secteur,buffer,HDA_SECTORSIZE);
}
void frmt(){
  int i,j;
  unsigned char buffer[HDA_SECTORSIZE];

  for(i=0;i<HDA_SECTORSIZE;i++){
    buffer[i]= 0 ;
  }
  for(i=0;i<HDA_MAXCYLINDER;i++){
    for(j=0;j<HDA_MAXSECTOR;j++){
      write_sector(i,j,buffer);
    }
  }
  if(verbose)
    printf("\nLe formatage s'est déroulé avec succés\n");
}





void format_sector(unsigned int piste,unsigned int secteur,unsigned int nsecteur,unsigned int value){
  int i;
  unsigned char buffer[HDA_SECTORSIZE];
  for(i=0;i<HDA_SECTORSIZE;i++)
    buffer[i] = value;

  for(i=secteur;i< secteur+ nsecteur;i++)
    write_sector(piste,i,buffer);

  if(verbose)
    printf("\nLe formatage du secteur s'est déroulé avec succés\n");


}











void empty_it(){}









void programmeTest(){

  int i,j;
  unsigned char buffer[HDA_SECTORSIZE];
  initialisation();
  printf("Test de dmps() : \n");
  for(i=0;i<HDA_MAXCYLINDER;i++)
    for(j=0;j<HDA_MAXSECTOR;j++)
      dmps(i,j);
  printf("\nTest de frmt() : \n");
  for(i=0;i<HDA_MAXCYLINDER;i++)
    for(j=0;j<HDA_MAXSECTOR;j++)
      dmps(i,j);
  init_hardware("hardware.ini");
  printf("\n\n Réinitialisation du disque... \n");
  printf("\n\nTest de read_sector() : \n");

  for(i=0;i<HDA_MAXCYLINDER;i++){
    for(j=0;j<HDA_MAXSECTOR;j++){
      read_sector(i,j,buffer);
    }
    for(j=0;j<HDA_SECTORSIZE;j++)
      printf("%x ",buffer[i]);
    printf("\n");
  }
  printf("\n\nTest de write_sector() : \n");
  for(i=0;i<HDA_MAXCYLINDER;i++){
    for(j=0;j<HDA_MAXSECTOR;j++){
      write_sector(i,j,buffer);
    }
    for(j=0;j<HDA_SECTORSIZE;j++)
      printf("%x ",buffer[i]);
    printf("\n");
  }
  printf("\n\nTest de format_sector() : \n");
  for(i=0;i<HDA_MAXCYLINDER;i++){
    for(j=0;j<HDA_SECTORSIZE;j++)
      buffer[j]=i;
    for(j=0;j<HDA_MAXSECTOR;j++){
      write_sector(i,j,buffer);
    }
    for(j=0;j<HDA_SECTORSIZE;j++)
      printf("%x ",buffer[i]);
    printf("\n");
  }

}
