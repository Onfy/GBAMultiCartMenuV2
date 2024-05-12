#include "manage.h"
#include <gba_console.h>
#include "misc.h"
#include <stdio.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include "flash.h"
#include <string.h>
#include "menu.h"

const int MAGIC_LEN = 32;
const char MAGIC_CODE_STD[MAGIC_LEN] = "THIS IS A TEST VER";
const int META_BLOCK_IDX = 1;//Store metadata at 256KB (it seems the value is a multiple of 256KB)
struct LastTimeRun{
    char MAGIC_CODE1[MAGIC_LEN];
    char gameName[GAME_NAME_LEN + 1];
    int MBOffset;
    bool load_from_auto_save;
    char MAGIC_CODE2[MAGIC_LEN];
    LastTimeRun(const volatile LastTimeRun& that){
        this->MBOffset = that.MBOffset;
        this->load_from_auto_save = that.load_from_auto_save;
        for(int i=0;i<MAGIC_LEN;i++){
            this->MAGIC_CODE1[i] = that.MAGIC_CODE1[i];
            this->MAGIC_CODE2[i] = that.MAGIC_CODE2[i];
        }
        for(int i=0;i<GAME_NAME_LEN+1;i++){
            this->gameName[i] = that.gameName[i];
        }
    }
    bool isValid(){
        int cmp1 = strncmp(MAGIC_CODE1,MAGIC_CODE_STD,MAGIC_LEN);
        int cmp2 = strncmp(MAGIC_CODE2,MAGIC_CODE_STD,MAGIC_LEN);
        return (cmp1 == 0 && cmp2 == 0);
    }
    LastTimeRun (int _MBOffset){
        this->MBOffset = _MBOffset;
        this->load_from_auto_save = false;
        strncpy(MAGIC_CODE1,MAGIC_CODE_STD,MAGIC_LEN);
        strncpy(MAGIC_CODE2,MAGIC_CODE_STD,MAGIC_LEN);
    }
};

void saveMetaToFlash(LastTimeRun newLastRun){

    LastTimeRun* lastRunBuffer = (LastTimeRun*)globle_buffer;
    *lastRunBuffer = newLastRun;//新的Meta
    unlockBlock(META_BLOCK_IDX);
    eraseBlock(META_BLOCK_IDX);
    flashIntelBuffered(META_BLOCK_IDX,0,1);//烧写Meta
}

int askMBOffset(int lastOffset){

    findGames();
    Menu gameMenu("========Games=======");
    for(int i=0;i<gameCnt;i++){
        gameMenu.addOption(std::to_string(gameEntries[i].MB_offset) +std::string("MB  ") +std::string(gameEntries[i].name));
    }
    int option = gameMenu.getDecision();
    while(option == -1){
        option = gameMenu.getDecision();
    }
    int offset = gameEntries[option].MB_offset;    

    LastTimeRun newLastRun(offset);//建立新的meta
    strncpy(newLastRun.gameName, gameEntries[option].name,GAME_NAME_LEN);
    newLastRun.gameName[GAME_NAME_LEN]='\0';

    saveMetaToFlash(newLastRun);

    gotoChipOffset(offset,true,false);//开始游戏
    return offset;
}

bool autoStartGame(){
    gotoChipOffset(0,false,false);
    LastTimeRun last_run = *(volatile LastTimeRun*)(GAME_ROM + META_BLOCK_IDX * BLOCK_SIZE); 
    if(last_run.isValid()){
        gotoChipOffset(last_run.MBOffset,true,true);
        return true;//should never return
    }
    return false;
}


bool pressedKeyOnBoot(u16 key){
    for(int i=0;i<1024;i++){
        scanKeys();
        if(keysDownRepeat() & key){
            return true;
        }
    }
    return false;
}
int trySaveGame(){

    LastTimeRun last_run = *(volatile LastTimeRun*)(GAME_ROM + META_BLOCK_IDX * BLOCK_SIZE); 
    if(last_run.isValid()){
        std::string gameInfoStr = "Last Game:\nName: ";
        gameInfoStr += (const char*)last_run.gameName;
        gameInfoStr += "\nOffset: " +std::to_string(last_run.MBOffset)+std::string("MB");
        // pressToContinue(true);
        std::string menuTitle = "\n" + gameInfoStr + "\n\nPress A to continue\n";
        Menu menu(menuTitle.c_str());
        menu.addOption("Ok");
        int option = menu.getDecision();
        if(option == 0){
            printf("\n");
        }
        //Silly hack to prevent the screen instantly clearing by holding the shoulder button. Sorry.
        return last_run.MBOffset;
    }
    return -1;
}
