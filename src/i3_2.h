// prool:

#define PULSE_PER_SECOND	4

int DEBUG =0;
extern DESCRIPTOR_DATA *descriptor_list;
typedef class Character CHAR_DATA;
extern time_t boot_time;

#define SAFE_NAME(ch)    ((ch)->get_name()) // prool: from vmud

#define CREATE_VOID(result, type, number)				\
do									\
{									\
  if (!((result) = (void *) calloc ((number), sizeof(type))))		\
  {									\
    perror("calloc failure");						\
    fprintf(stderr, "Calloc failure @ %s:%d\n", __FILE__, __LINE__ );	\
    fflush(stderr);                                                     \
    proper_exit(42);							\
  }									\
} while(0)

#define STRDUP(result, string)						\
do									\
{									\
  if (!((result) = (char *) calloc (strlen(string), sizeof(char))))	\
  {									\
    perror("calloc failure");						\
    fprintf(stderr, "Calloc failure @ %s:%d\n", __FILE__, __LINE__ );	\
    fflush(stderr);                                                     \
    proper_exit(42);							\
  }									\
  strcpy((result), (string));						\
} while(0)

#define DESTROY(point)							\
do									\
{									\
  if((point))								\
  {									\
    free((point));							\
    (point) = NULL;							\
  }									\
} while(0)

/* double-linked list handling macros -Thoric ( From the Smaug codebase ) */
/* Updated by Scion 8/6/1999 */
#define LINK(link, first, last, next, prev)  \
do                                              \
{                                               \
   if ( !(first) )                              \
   {                                            \
      (first) = (link);                         \
      (last) = (link);                          \
   }                                            \
   else                                         \
      (last)->next = (link);                    \
   (link)->next = NULL;                         \
   if ((first) == (link))                       \
      (link)->prev = NULL;                      \
   else                                         \
      (link)->prev = (last);                    \
   (last) = (link);                             \
} while(0)

#define INSERT(link, insert, first, next, prev)    \
do                                                    \
{                                                     \
   (link)->prev = (insert)->prev;                     \
   if ( !(insert)->prev )                             \
      (first) = (link);                               \
   else                                               \
      (insert)->prev->next = (link);                  \
   (insert)->prev = (link);                           \
   (link)->next = (insert);                           \
} while(0)

#define UNLINK(link, first, last, next, prev) \
do                                               \
{                                                \
   if ( !(link)->prev )                          \
   {                                             \
      (first) = (link)->next;                    \
	if((first))                                \
	   (first)->prev = NULL;                   \
   }                                             \
   else                                          \
   {                                             \
      (link)->prev->next = (link)->next;         \
   }                                             \
   if( !(link)->next )                           \
   {                                             \
      (last) = (link)->prev;                     \
	if((last))                                 \
	   (last)->next = NULL;                    \
   }                                             \
   else                                          \
   {                                             \
      (link)->next->prev = (link)->prev;         \
   }                                             \
} while(0)

#define FCLOSE(fp) \
do {               \
  if((fp))         \
    fclose(fp);    \
  fp = NULL;       \
} while(0)
