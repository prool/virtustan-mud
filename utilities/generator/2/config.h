// zone generator 0.1 by prool@itl.ua

// config

#define ZONE_START 6200
#define ZONE_END (ZONE_START+9)
#define ZONE_NAME "Каменистая пустошь"
#define REPOP_TIME 90 // minutes
#define REPOP_TYPE 2 // repop: 0 - none, 1 - no gamers, 2 - always

#define MOB_IFFLAG "0" // mob ifflag for zon file, string

// параметры мобов

#define ALIAS "дракон drakon"
#define PADEZH1 "дракон"
#define PADEZH2 "дракона"
#define PADEZH3 "дракону"
#define PADEZH4 "дракона"
#define PADEZH5 "драконом"
#define PADEZH6 "драконе"
#define DESCR "В воздухе парит изумрудный дракон"
#define LONG_DESCR "Драконы древние магические существа.\n"
#define MOB_FLAG "b0" // f0 - agressive, b0 - not walking
#define MOB_AFF "u1"
#define ALIGN 0 // 0 - neutral, -900 - evil, +900 - good
#define MOB_LVL 50 
#define MOB_HITROLL 0 // Thac0 (чем меньше, тем лучше моб попадает)
#define MOB_AC -10 
#define MOB_HIT "15d15+20000"
#define MOB_DAMAGE "8d25+1500"
#define MOB_MONEY "2d2+1"
#define MOB_EXPERIENSE 2000000
#define MOB_POS_BOOT 8 // 8 - стоит
#define MOB_POS_DEFAULT 8
#define MOB_SEX 1 // 1 - male
#define MOB_PARAMETERS "BareHandAttack: 5\n\
Str: 15\n\
Int: 15\n\
Wis: 15\n\
Dex: 14\n\
Con: 15\n\
Cha: 18\n\
LikeWork: 0\n\
MaxFactor: 0\n\
ExtraAttack: 255\n\
Class: 103\n\
Size: 60\n\
Height: 150\n\
Weight: 10\n\
Special_Bitvector: a1b1c1d1\n\
Skill: 134 90\n\
Skill: 136 90" // у последнего параметра \n не ставится
// object to mob
#define OBJ_IFFLAG 1
#define OBJ_VNUM 200
#define OBJ_MAX_IN_WORLD 1000
#define OBJ_VEROYATN 100
#define OBJ_NADET -1

// параметры комнат
#define ROOM_TITLE "Каменистая пустошь"
#define ROOM_DESCR1 "Вокруг простирается каменистая пустошь. Кое-где поднимается дымок" // plain
#define ROOM_DESCR2 "   Вы парите над равниной на высоте %i м. От ощущения высоты захватывает дух" // in fly
#define ROOM_FLAGS 0
#define ROOM_SECTOR_TYPE 2
#define ROOM_EXIT_FLAG 0
#define ROOM_EXIT_KEY -1
