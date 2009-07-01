#ifndef  YYSTYPE
#define  YYSTYPE  int
#endif

#define IDENTIFIER 257
#define INTEGER_CONST 258
#define FLOAT_CONST 259
#define STRING_CONST 260
#define ADDEQ 261
#define SUBEQ 262
#define MULEQ 263
#define DIVEQ 264
#define MODEQ 265
#define XOREQ 266
#define ANDEQ 267
#define OREQ 268
#define SL 269
#define SR 270
#define USR 271
#define SLEQ 272
#define SREQ 273
#define USREQ 274
#define EQ 275
#define NOTEQ 276
#define LTEQ 277
#define GTEQ 278
#define ANDAND 279
#define OROR 280
#define PLUSPLUS 281
#define MINUSMINUS 282
#define FLOAT_TYPE 283
#define INT_TYPE 284
#define OBJECT_TYPE 285
#define STRING_TYPE 286
#define VOID_TYPE 287
#define STRUCT_TYPE 288
#define VECTOR_TYPE 289
#define ENGINE_TYPE 290
#define ACTION_TYPE 291
#define OBJECT_SELF_CONST 292
#define OBJECT_INVALID_CONST 293
#define BREAK 294
#define CASE 295
#define CONTINUE 296
#define DEFAULT 297
#define DO 298
#define ELSE 299
#define FOR 300
#define IF 301
#define RETURN 302
#define SWITCH 303
#define WHILE 304
#define CONST 305

class yyparser{
  int yyexpand_states_stack (int **start, int **end, int **top);
  int yyexpand_attributes_stack (YYSTYPE **start, YYSTYPE **end, YYSTYPE **top);

public:
  YYSTYPE  yylval;

  int yychar;

  int yydebug;

  virtual int yylex (void) = 0;
  virtual void yyerror (const char *message) = 0;
  int yyparse (void);
  yyparser (void) {}
  virtual ~yyparser (void)  {}
};


/* vim: set sw=4: */
