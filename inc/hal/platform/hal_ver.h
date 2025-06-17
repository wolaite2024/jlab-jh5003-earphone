#define VERSION_MAJOR            3
#define VERSION_MINOR            14
#define VERSION_REVISION         4
#define VERSION_BUILDNUM         1706
#define VERSION_GCID             0xe3c20ab3
#define CUSTOMER_NAME            sdk-v1
#define CN_1                     's'
#define CN_2                     'd'
#define CN_3                     'k'
#define CN_4                     '-'
#define CN_5                     'v'
#define CN_6                     '1'
#define CN_7                     '#'
#define CN_8                     '#'
#define BUILDING_TIME            Mon Nov  4 12:21:12 2024
#define NAME2STR(a)              #a
#define CUSTOMER_NAME_S          #NAME2STR(CUSTOMER_NAME)
#define NUM4STR(a,b,c,d)         #a "." #b "." #c "." #d
#define VERSIONBUILDSTR(a,b,c,d) NUM4STR(a,b,c,d)
#define VERSION_BUILD_STR        VERSIONBUILDSTR(VERSION_MAJOR,VERSION_MINOR,VERSION_REVISION,VERSION_BUILD)
#define COMMIT                   e3c20ab379d6
#define BUILDING_TIME_STR        Mon_2024_11_04_12_21_12
#define BUILDER                  leo825x
#define BUILDER_STR              leo825x
#define TO_STR(R) NAME2STR(R)
#define GENERATE_VERSION_MSG(MSG, VERSION, COMMIT, BUILDING_TIME, BUILDER) \
    GENERATE_VERSION_MSG_(MSG, VERSION, COMMIT, BUILDING_TIME, BUILDER)
#define GENERATE_VERSION_MSG_(MSG, VERSION, COMMIT, BUILDING_TIME, BUILDER) \
    MSG##_##VERSION##_##COMMIT##_##BUILDING_TIME##_##BUILDER

#define GENERATE_BIN_VERSION(MSG, VERSION) \
    typedef char GENERATE_VERSION_MSG(MSG, VERSION, COMMIT, BUILDING_TIME_STR, BUILDER_STR);
