#ifndef DEF_H
#define DEF_H

#include <QString>

#define VAM_TYPE_TOTAL 11

enum Thread_Run_Type
{
    RUN_NONE,
    RUN_STAT,
    RUN_LIST,
    RUN_QUERY_LIST,
    RUN_VAR_VALID,
    RUN_VAR_SAME,
    RUN_ITEM_PIC,
    RUN_VIEW_IN_VAR,
};

enum eVamType
{
    TYPE_UNKNOWN=0,
    TYPE_SCENE,
    TYPE_ASSET,
    TYPE_CLOTH,
    TYPE_HAIR,
    TYPE_APPEAR,
    TYPE_POSE,
    TYPE_SCRIPT,
    TYPE_TEXTURE,
    TYPE_VAR,
    TYPE_OTHER,
};

enum Gender
{
    GEN_FEMALE=0,
    GEN_MALE,
    GEN_ALL,
};

struct extType
{
    QString ext;
    QString path;
    int sum;
};

struct VAM_ITEM
{
    QString name;
    QString filePath;
    QString orgFilePath;
    QString prevFilePath;
    int gender;
    bool inVar;
    QString author;
    long fileSize;
    QString fileTime;
};

struct FILE_IN_VAR
{
    QString fileName;
    QString parentVarName;
    QString fileTime;
    long fileSize;
};

enum getSQLType
{
    GET_INSERT,
    GET_SET,
    GET_SELECT,
};

struct VAR_INFO
{
    QString path;
    QString ftitle;
    QString fext;
    int size;
    int flag;
};

enum VamItemField
{
    F_ID=0,
    F_VAM_GAME_ID,
    F_NAME,
    F_VAM_TYPE,
    F_GENDER,
    F_IN_VAR,
    F_FILE_SIZE,
    F_FILE_PATH,
    F_PREV_FILE_PATH,
    F_PREV_FILE_LOC,
    F_ORG_FILE_PATH,
    F_AUTHOR,
    F_VAM_FLAG,
    F_DESCRIPT,
    F_SCORE,
    F_BIRTH_TIME,
    F_RECORD_TIME,
    F_CHECKED,
};


#endif // DEF_H
