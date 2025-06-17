#define VERSION_MAJOR            3
#define VERSION_MINOR            14
#define VERSION_REVISION         5
#define VERSION_BUILDNUM         1420
#define VERSION_GCID             0xbc60a0df
#define CUSTOMER_NAME            sdk-v1
#define CN_1                     's'
#define CN_2                     'd'
#define CN_3                     'k'
#define CN_4                     '-'
#define CN_5                     'v'
#define CN_6                     '1'
#define CN_7                     '#'
#define CN_8                     '#'
#define BUILDING_TIME            Tue Jun 10 14:37:37 2025
#define NAME2STR(a)              #a
#define CUSTOMER_NAME_S          #NAME2STR(CUSTOMER_NAME)
#define NUM4STR(a,b,c,d)         #a "." #b "." #c "." #d
#define VERSIONBUILDSTR(a,b,c,d) NUM4STR(a,b,c,d)
#define VERSION_BUILD_STR        VERSIONBUILDSTR(VERSION_MAJOR,VERSION_MINOR,VERSION_REVISION,VERSION_BUILD)
#define COMMIT                   bc60a0df6dd6
#define BUILDING_TIME_STR        Tue_2025_06_10_14_37_37
#define BUILDER                  btsw1_jenkins_no_reply
#define BUILDER_STR              btsw1_jenkins_no_reply
#define TO_STR(R) NAME2STR(R)
#define GENERATE_VERSION_MSG(MSG, VERSION, COMMIT, BUILDING_TIME, BUILDER) \
    GENERATE_VERSION_MSG_(MSG, VERSION, COMMIT, BUILDING_TIME, BUILDER)
#define GENERATE_VERSION_MSG_(MSG, VERSION, COMMIT, BUILDING_TIME, BUILDER) \
    MSG##_##VERSION##_##COMMIT##_##BUILDING_TIME##_##BUILDER

#define GENERATE_BIN_VERSION(MSG, VERSION) \
    typedef char GENERATE_VERSION_MSG(MSG, VERSION, COMMIT, BUILDING_TIME_STR, BUILDER_STR);
