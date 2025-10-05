#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define e 2.7182818284
#define p 3.1415926535

typedef struct parser_t {
    char* tokens;
    int ntokens;
    int pos;
} parser_t;

typedef struct hesaplamaagaci
 {
    char type;
    double value;
    struct hesaplamaagaci* left;
    struct hesaplamaagaci* right;
} hesaplamaagaci;

#define VALID_TOKENS "+-*/^_0123456789()?elogarcsitnx"
#define MAX_INPUT_SIZE 100

double variable = 0;

char* tokenize(char*);
hesaplamaagaci* parse(char*,double);
double calculate(hesaplamaagaci*,double);
void free_hesaplamaagaci(hesaplamaagaci*);
hesaplamaagaci* create_hesaplamaagaci(char, double, hesaplamaagaci*, hesaplamaagaci*);
hesaplamaagaci* parse_add_expression(parser_t*,double);
hesaplamaagaci* parse_mult_expression(parser_t*,double);
hesaplamaagaci* parsepowexpression(parser_t*,double);
hesaplamaagaci* parse_log_expression(parser_t*,double);
hesaplamaagaci* parse_atomic_expression(parser_t*,double);
hesaplamaagaci* parse_number(parser_t*,double);
hesaplamaagaci* parse_function_expression(parser_t*,double);
/*
@brief Metni gecerli token dizisine cevirir. Negatif sayi ve ondalik noktayi guvenli isler.
@param in Parametre.
@return Heap uzerinde ayrilmis token dizisi (char*).
*/
char* tokenize(char* in) {
    char* tokens = malloc(sizeof(char) * MAX_INPUT_SIZE);
    int token_pos = 0;
    int dot_encountered = 0;
    int in_len = strlen(in);
    int i;
    for (i = 0; i < in_len; i++) {
        if (strchr(VALID_TOKENS, in[i])) {
            tokens[token_pos++] = in[i];
            dot_encountered = 0;
        } else if (in[i] == '.') {
            if (!dot_encountered && ((i > 0 && strchr("0123456789", in[i - 1])) && (i < in_len - 1 && strchr("0123456789", in[i + 1])))) {
                tokens[token_pos++] = in[i]; 
                dot_encountered = 1; 
            } else {
                fprintf(stderr, "Invalid input\n");
                exit(1);
            }
        } else if (in[i] == '-' && (i == 0 || in[i - 1] == '(')) { 
            tokens[token_pos++] = '0'; 
            tokens[token_pos++] = in[i]; 
        } else {
            fprintf(stderr, "Invalid input\n");
            exit(1);
        }
    }
    tokens[token_pos] = '\0';
    return tokens;
}

/*
@brief Token dizisini hesaplama agacina (AST) parse eder.
@param tokens Parametre.
@param variable Parametre.
@return Kok dugum isaretcisi (hesaplamaagaci*).
*/
hesaplamaagaci* parse(char* tokens,double variable) {
    parser_t* parser = malloc(sizeof(parser_t));
    parser->tokens = tokens;
    parser->ntokens = strlen(tokens);
    parser->pos = 0;
    hesaplamaagaci* expression = parse_add_expression(parser,variable);
    free(parser->tokens);
    free(parser);
    return expression;
}

/*
@brief Hesaplama agacini ve x degerini kullanarak ifadeyi degerlendirir.
@param expr Parametre.
@param variable Parametre.
@return Ifadenin double sonucu.
*/
double calculate(hesaplamaagaci* expr,double variable) {
    if (expr->type == 'n')
        return expr->value;//say ise direkt kendini dndryor

    double left = expr->left ? calculate(expr->left,variable) : 0;// NULL kontrol
    double right = expr->right ? calculate(expr->right,variable) : 0;

    switch (expr->type) {
        case '+':
            return left + right;
        case '-':
            return left - right;
        case '*':
            return left * right;
        case '/':
            if (right != 0) {
                return left / right;
            } else {
                fprintf(stderr, "MATH ERROR: Division by zero\n");
                exit(1);
            }
        case '^':
            return pow(left, right);
        case 'l':
            if (right > 0 && left > 1) {
                return log(right) / log(left);
            } else {
                fprintf(stderr, "MATH ERROR: Invalid logarithm\n");
                exit(1);
            }
        case 'a': 
            if (right >= -1 && right <= 1) {
                return asin(right);
            } else {
                fprintf(stderr, "MATH ERROR: Invalid arcsin input\n");
                exit(1);
            }
        case 'b': 
            if (right >= -1 && right <= 1) {
                return acos(right);
            } else {
                fprintf(stderr, "MATH ERROR: Invalid arccos input\n");
                exit(1);
            }
        case 'c': 
            return atan(right);
        case 'd': 
            return atan(1.0 / right);
        case 'e': 
            if (fabs(right) >= 1) {
                return acos(1.0 / right);
            } else {
                fprintf(stderr, "MATH ERROR: Invalid arcsec input\n");
                exit(1);
            }
        case 'f': 
            if (fabs(right) >= 1) {
                return asin(1.0 / right);
            } else {
                fprintf(stderr, "MATH ERROR: Invalid arccsc input\n");
                exit(1);
            }
        case 'g': 
            return sin(right); 
        case 'h': 
            return cos(right); 
        case 'i': 
            if (fmod(right, p) != p/2) {
                return tan(right);// trigonometrik fonksiyonlarda derece kullanmay tercih ettim.
            } else {
                fprintf(stderr, "MATH ERROR: Invalid tan input\n");
                exit(1);
            }
        case 'j': 
            if (fmod(right, p) != 0) {
                return 1.0 / tan(right);
            } else {
                   fprintf(stderr, "MATH ERROR: Invalid tan input\n");
                   exit(1);
            }
        case 'k': 
            if (fmod(right, p) != p / 2) {
                return 1.0 / cos(right); 
            } else {
                fprintf(stderr, "MATH ERROR: Invalid sec input\n");
                exit(1);
            }
        case 'm': 
            if (fmod(right, p) != 0) {
                return 1.0 / sin(right * M_PI / 180);
            } else {
                fprintf(stderr, "MATH ERROR: Invalid csc input\n");
                exit(1);
            }
        default:
            fprintf(stderr, "MATH ERROR: Unknown expression type\n");
            exit(1);
    }
    return 0;
}
/*
@brief Toplama/cikarma seviyesindeki ifadeleri parse eder. Unary '-' destekler.
@param parser Parametre.
@param variable Parametre.
@return Alt agac koku.
*/
hesaplamaagaci* parse_add_expression(parser_t* parser,double variable) {
    int unary_minus = 0;
    if (parser->tokens[parser->pos] == '-') {
        unary_minus = 1;
        parser->pos++;// kodun bu ksmnda bir saynn bandaki - iaretinin karma ilemi iareti mi yoksa negatiflik iareti mi olduunu belirler.
    }
    hesaplamaagaci* expr = parse_mult_expression(parser,variable);
    if (unary_minus) {
        hesaplamaagaci* zero_expr = create_hesaplamaagaci('n', 0, NULL, NULL);
        expr = create_hesaplamaagaci('-', 0, zero_expr, expr);
    }
    while (parser->pos < parser->ntokens &&
            (parser->tokens[parser->pos] == '+' || parser->tokens[parser->pos] == '-')) {
        char type = parser->tokens[parser->pos];
        parser->pos++;
        hesaplamaagaci* right_expr = parse_mult_expression(parser,variable);// kodun bu ksmnda da arpma ilemi ve blme ilemi daha ncelikli olduu iin saa arpma ilemini attm.
        expr = create_hesaplamaagaci(type, 0, expr, right_expr);
    }
    return expr; 
}
/*
@brief Carpma/bolme seviyesindeki ifadeleri parse eder.
@param parser Parametre.
@param variable Parametre.
@return Alt agac koku.
*/
hesaplamaagaci* parse_mult_expression(parser_t* parser,double variable) {
    hesaplamaagaci* expr = parsepowexpression(parser,variable);
    while (parser->pos < parser->ntokens &&
            (parser->tokens[parser->pos] == '*' || parser->tokens[parser->pos] == '/')) {
        char type = parser->tokens[parser->pos];
        parser->pos++;
        hesaplamaagaci* right_expr = parsepowexpression(parser,variable); // kodun bu ksmnda s alma ilemi arpmadan ve blmeden ncelikli olduu iin s alma ilemini saa atyoruz
        expr = create_hesaplamaagaci(type, 0, expr, right_expr);
    }
    return expr;
}
/*
@brief Us alma (^) seviyesindeki ifadeleri parse eder.
@param parser Parametre.
@param variable Parametre.
@return Alt agac koku.
*/
hesaplamaagaci* parsepowexpression(parser_t* parser,double variable) {
    hesaplamaagaci* expr = parse_log_expression(parser,variable);
    while (parser->pos < parser->ntokens &&
            (parser->tokens[parser->pos] == '^')) {
        char type = parser->tokens[parser->pos];
        parser->pos++;
        hesaplamaagaci* right_expr = parse_log_expression(parser,variable); // logaritma ilemi s alma ileminden ncelikli olduu iin logaritmay saa attm
        expr = create_hesaplamaagaci(type, 0, expr, right_expr);
    }
    return expr;
}
/*
@brief Siradaki tokenlarin sabit dize ile eslesmesini kontrol eder ve pos'u ilerletir.
@param parser Parametre.
@param str Parametre.
@return Eslesirse 1, aksi halde 0.
*/
int match_token(parser_t* parser, const char* str) {
    int len = strlen(str);// bu fonksiyonun amac say ve baz sabitler dnda bie eyle karlatnda bunu tutmas sonuta log_ sin gibi fonksiyonlar anlamam lazm
    int i;
    for (i = 0; i < len; ++i) {
        if (parser->tokens[parser->pos + i] != str[i]) {
            return 0;
        }
    }
    parser->pos += len;
    return 1;
}
/*
@brief log_(base)(expr) bicimindeki logaritma ifadelerini parse eder.
@param parser Parametre.
@param variable Parametre.
@return Logaritma veya ic ifade agaci.
*/
hesaplamaagaci* parse_log_expression(parser_t* parser,double variable) {
    if (match_token(parser, "log_")) {
        hesaplamaagaci* base_expr;
        if (parser->tokens[parser->pos] == '(') {// burada paranteze gre kontrol etmemin sebebi tabana ilemli bir ifade yazlrsa doru bir ekilde ayrt edebilmek iin
            parser->pos++;
            base_expr = parse_add_expression(parser, variable);
            if (parser->tokens[parser->pos] == ')') {
                parser->pos++;
            } else {
                fprintf(stderr, "Invalid input: expected ')'\n");
                exit(1);
            }
        } else {
            fprintf(stderr, "Invalid input: expected '('\n");
            exit(1);
        }
        if (parser->tokens[parser->pos] == '(') {
            parser->pos++;
            hesaplamaagaci* inner_expr = parse_add_expression(parser, variable);// burada saylar ve ilemleri kontrol ediyor ilem nceliine gre sadakileri nce okuyacak
            if (parser->tokens[parser->pos] == ')') {
                parser->pos++;
                return create_hesaplamaagaci('l', 0, base_expr, inner_expr);
            } else {
                fprintf(stderr, "Invalid input: expected ')'\n");
                exit(1);
            }
        } else {
            fprintf(stderr, "Invalid input: expected '('\n");
            exit(1);
        }
    } else {
        return parse_function_expression(parser, variable); // eer ki buradaki hibiri deilse trigonometrik fonksiyon vardr iinde o yzden trigonometrik fonksiyonu kontrol ediyor trigonometrik fonksiyonda da parantez iinde yazlanlar kontrol eder bu sayede her ilemi alm olur
    }
}
/*
@brief Trigonometrik ve ters trig. fonksiyonlari (sin, cos, tan, arcsin, arccos, arctan, sec, csc, arcsec) parse eder.
@param parser Parametre.
@param variable Parametre.
@return Fonksiyon agaci.
*/
hesaplamaagaci* parse_function_expression(parser_t* parser,double variable) {
    if (match_token(parser, "arccos(")) {
        hesaplamaagaci* inner_expr = parse_add_expression(parser,variable);
        if (parser->tokens[parser->pos] == ')') {
            parser->pos++;
            return create_hesaplamaagaci('b', 0, NULL, inner_expr);//ilk index sayesinde hangi ilem olduunu rahata anlayabiliyoruz
        } else {
            fprintf(stderr, "Invalid input: expected ')'\n");
            exit(1);
        }
    } else if (match_token(parser, "arctan(")) {
        hesaplamaagaci* inner_expr = parse_add_expression(parser,variable);
        if (parser->tokens[parser->pos] == ')') {
            parser->pos++;
            return create_hesaplamaagaci('d', 0, NULL, inner_expr);
        } else {
            fprintf(stderr, "Invalid input: expected ')'\n");
            exit(1);
        }
    } else if (match_token(parser, "arcsec(")) {
        hesaplamaagaci* inner_expr = parse_add_expression(parser,variable);
        if (parser->tokens[parser->pos] == ')') {
            parser->pos++;
            return create_hesaplamaagaci('f', 0, NULL, inner_expr);
        } else {
            fprintf(stderr, "Invalid input: expected ')'\n");
            exit(1);
        }
    } else if (match_token(parser, "arcsin(")) {
        hesaplamaagaci* inner_expr = parse_add_expression(parser,variable);
        if (parser->tokens[parser->pos] == ')') {
            parser->pos++;
            return create_hesaplamaagaci('a', 0, NULL, inner_expr);
        } else {
            fprintf(stderr, "Invalid input: expected ')'\n");
            exit(1);
        }
    } else if (match_token(parser, "cos(")) {
        hesaplamaagaci* inner_expr = parse_add_expression(parser,variable);
        if (parser->tokens[parser->pos] == ')') {
            parser->pos++;
            return create_hesaplamaagaci('h', 0, NULL, inner_expr);
        } else {
            fprintf(stderr, "Invalid input: expected ')'\n");
            exit(1);
        }
    } else if (match_token(parser, "tan(")) {
        hesaplamaagaci* inner_expr = parse_add_expression(parser,variable);
        if (parser->tokens[parser->pos] == ')') {
            parser->pos++;
            return create_hesaplamaagaci('i', 0, NULL, inner_expr);
        } else {
            fprintf(stderr, "Invalid input: expected ')'\n");
            exit(1);
        }
    } else if (match_token(parser, "sec(")) {
        hesaplamaagaci* inner_expr = parse_add_expression(parser,variable);
        if (parser->tokens[parser->pos] == ')') {
            parser->pos++;
            return create_hesaplamaagaci('k', 0, NULL, inner_expr);
        } else {
            fprintf(stderr, "Invalid input: expected ')'\n");
            exit(1);
        }
    } else if (match_token(parser, "sin(")) {
        hesaplamaagaci* inner_expr = parse_add_expression(parser,variable);
        if (parser->tokens[parser->pos] == ')') {
            parser->pos++;
            return create_hesaplamaagaci('g', 0, NULL, inner_expr);}
            else {
            fprintf(stderr, "Invalid input: expected ')'\n");
            exit(1);
        }
    } else {
        return parse_atomic_expression(parser,variable);// burda ilem olarak hibir ey kalmadysa artk en ncelikli olan parantezlere geiyor
    }
}
/*
@brief Parantezli alt ifade veya sayi/degisken gibi atomigi parse eder.
@param parser Parametre.
@param variable Parametre.
@return Atomik ifade agaci.
*/
hesaplamaagaci* parse_atomic_expression(parser_t* parser,double variable) {
    hesaplamaagaci* expr;
    if (parser->tokens[parser->pos] == '(') {
        parser->pos++;
        expr = parse_add_expression(parser,variable);
        if (parser->tokens[parser->pos] == ')') {
            parser->pos++;
        } else {
            fprintf(stderr, "Invalid input: expected ')'\n");
            exit(1);
        }
    } else {
        expr = parse_number(parser,variable);// bu satr paranteze sadece say girilirse hata vermemesi iin
    }
    return expr;
}
/*
@brief Sayi, e, pi (?) ve x degiskenini ayristirir; double deger ureten dugum dondurur.
@param parser Parametre.
@param variable Parametre.
@return Sayi dugumu.
*/
hesaplamaagaci* parse_number(parser_t* parser,double variable) {
    char number[MAX_INPUT_SIZE];
    int numberlen = 0;
    int dot_encountered = 0;
    double value;
    while ((isdigit(parser->tokens[parser->pos]) || parser->tokens[parser->pos] == '.' || parser->tokens[parser->pos] == 'e' || parser->tokens[parser->pos] == '?'|| parser->tokens[parser->pos] == 'x') &&
           numberlen < MAX_INPUT_SIZE && parser->pos < parser->ntokens) {

        if (parser->tokens[parser->pos] == '.') {// buras double deerleri tutabilmek iin
            if (!dot_encountered) {
                number[numberlen++] = parser->tokens[parser->pos];
                dot_encountered = 1;
            } else {
                fprintf(stderr, "Invalid input, multiple dots in number\n");
                exit(1);
            }
        } else if (parser->tokens[parser->pos] == 'e') {// e saysnn tanmlanmas
            value = e;  
            hesaplamaagaci* number_expr = create_hesaplamaagaci('n', value, NULL, NULL);
            parser->pos++;  
            return number_expr;
        } else if (parser->tokens[parser->pos] == '?') {// pi saysnn tanmlanmas ? kullanmamn sebebi benim klavyemde pi saysnn olmamas
            value = p;  
            hesaplamaagaci* number_expr = create_hesaplamaagaci('n', value, NULL, NULL);
            parser->pos++;  
            return number_expr;
        } else if (parser->tokens[parser->pos] == 'x')  {
            value = variable;
            hesaplamaagaci* number_expr = create_hesaplamaagaci('n', value, NULL, NULL);
            parser->pos++;  
            return number_expr;
        } else {
            number[numberlen++] = parser->tokens[parser->pos];
            parser->pos++;
        }
    }
    number[numberlen] = '\0';
    value = atof(number);
    if (numberlen == 0) {
        fprintf(stderr, "Invalid input, couldn't parse number\n");
        exit(1);
    }
    hesaplamaagaci* number_expr = create_hesaplamaagaci('n', value, NULL, NULL);
    return number_expr;
}
/*
@brief Yeni bir hesaplama agaci dugumu olusturur.
@param type Parametre.
@param value Parametre.
@param left Parametre.
@param right Parametre.
@return Olusturulan dugum isaretcisi.
*/
hesaplamaagaci* create_hesaplamaagaci(char type, double value, hesaplamaagaci* left, hesaplamaagaci* right) {
    hesaplamaagaci* expr = malloc(sizeof(hesaplamaagaci));
    expr->type = type;
    expr->value = value;
    expr->left = left;
    expr->right = right;
    return expr;
}
/*
@brief Verilen hesaplama agacinin tum dugumlerini serbest birakir (post-order).
@param expr Parametre.
@return Deger dondurmez.
*/
void free_hesaplamaagaci(hesaplamaagaci* expr) {
    if (expr) {
        if (expr->left)
            free_hesaplamaagaci(expr->left);
        if (expr->right)
            free_hesaplamaagaci(expr->right);
        free(expr);
    }
}
/*
@brief Sayisal turev hesaplar: ileri (i), geri (g) veya merkezi (m) fark.
@param variable Parametre.
@param h Parametre.
@param igm Parametre.
@param in Parametre.
@return Turev yaklasimi (double).
*/
double sayisalturev( double variable, double h, char igm, char *in) {
   double result;
    char* tokens = tokenize(in);//parse ilemi
    hesaplamaagaci* expression = parse(tokens, variable);
    double f_x = calculate(expression, variable);//parse ilemi bittikten sonra ifadeyi hesaplyor
    double variable_plus_h = variable + h;
    char* tokens2 = tokenize(in);
    hesaplamaagaci* expression2 = parse(tokens2, variable_plus_h);
    double f_x_plus_h = calculate(expression2,variable_plus_h);
    double variable_minus_h = variable - h;
    char* tokens3 = tokenize(in);
    hesaplamaagaci* expression3 = parse(tokens3, variable_minus_h);
    double f_x_minus_h = calculate(expression3,variable_minus_h);
    // 3 n de parse edip sonucu ayr ayr tutmazsanz srekli 0 sonucuyla karlarsnz 
    if (igm == 'i') {
        result = (f_x_plus_h - f_x) / h;//ileri farkla trev
    } else if (igm == 'g') {
        result = (f_x - f_x_minus_h) / h;//geri farkla trev
    } else if (igm == 'm') {
        result = (f_x_plus_h - f_x_minus_h) / (2 * h);// merkezi farkla trev
    } else {
        fprintf(stderr, "Invalid method\n");
        exit(1);
    }
    return result;
    free_hesaplamaagaci(expression);
    free_hesaplamaagaci(expression2);
    free_hesaplamaagaci(expression3);
}
/*
@brief Trapez yontemi ile belirli integralin sayisal yaklasimini hesaplar.
@param limit1 Parametre.
@param limit2 Parametre.
@param n Parametre.
@param in Parametre.
@return Integral yaklasimi (double).
*/
double trapez(double limit1, double limit2, unsigned int n, char* in)
{
       int i=0;
       double result=0,sonuc1,sonuc2;
       double degisken1 = limit1 + i*(limit2 - limit1)/n;//x0
       double degisken2 = limit1 + (i + 1)*(limit2 - limit1)/n;//x1
       while(degisken2 != limit2)
       {
                       degisken1 = limit1 + i*(limit2 - limit1)/n;
                       char* tokens = tokenize(in);
                       hesaplamaagaci* expression = parse(tokens, degisken1);
                       sonuc1 = calculate(expression, degisken1);
                       degisken2 = limit1 + (i + 1)*(limit2 - limit1)/n;
                       char* tokens2 = tokenize(in);
                       hesaplamaagaci* expression2 = parse(tokens2, degisken2);
                       sonuc2 = calculate(expression2, degisken2);
                       printf("%lf-%lf : %lf-%lf\n",degisken1,degisken2,sonuc1 , sonuc2);
                       result = result + (sonuc1+sonuc2)/2;//trapez forml
                       free_hesaplamaagaci(expression);
                       free_hesaplamaagaci(expression2);
                       i++;
       }
       result*= (limit2-limit1)/n;
       return result;
}
/*
@brief Simpson 1/3 (o) veya 3/8 (l) kurallari ile sayisal integral alir.
@param limit1 Parametre.
@param limit2 Parametre.
@param n Parametre.
@param in Parametre.
@param decider Parametre.
@return Integral yaklasimi (double).
*/
double simpson(double limit1, double limit2, unsigned int n, char* in, char decider)
{
        if (n % 2 != 0 && decider == 'o') {
        fprintf(stderr, "Simpson's 1/3 rule requires an even number of intervals.\n");
        exit(1);
    }

    else if (n % 3 != 0 && decider == 'l') {
        fprintf(stderr, "Simpson's 3/8 rule requires a number of intervals divisible by 3.\n");
        exit(1);
    }
    else
      {double result = 0.0;
    double step = (limit2 - limit1) / n;
    if (decider == 'o') {
        unsigned int i;
        for (i = 0; i < n; i += 2) {
            double degisken1 = limit1 + i * step;//x0
            double degisken2 = limit1 + (i + 1) * step;//x1
            double degisken3 = limit1 + (i + 2) * step;//x2

            char* tokens1 = tokenize(in);
            hesaplamaagaci* expression1 = parse(tokens1, degisken1);
            double sonuc1 = calculate(expression1, degisken1);

            char* tokens2 = tokenize(in);
            hesaplamaagaci* expression2 = parse(tokens2, degisken2);
            double sonuc2 = calculate(expression2, degisken2);

            char* tokens3 = tokenize(in);
            hesaplamaagaci* expression3 = parse(tokens3, degisken3);
            double sonuc3 = calculate(expression3, degisken3);

            printf("%lf-%lf-%lf : %lf-%lf-%lf\n", degisken1, degisken2, degisken3, sonuc1 , sonuc2 , sonuc3);
            result += sonuc1 + 4 * sonuc2 + sonuc3;//bunda da forml y0+4y1+y2

            free_hesaplamaagaci(expression1);
            free_hesaplamaagaci(expression2);
            free_hesaplamaagaci(expression3);
            free(tokens1);
            free(tokens2);
            free(tokens3);
        }
        result *= step / 3;
    } else if (decider == 'l') {
           unsigned int i;
        for (i = 0; i < n; i += 3) {
            double degisken1 = limit1 + i * step;//x0
            double degisken2 = limit1 + (i + 1) * step;//x1
            double degisken3 = limit1 + (i + 2) * step;//x2
            double degisken4 = limit1 + (i + 3) * step;//x3

            char* tokens1 = tokenize(in);
            hesaplamaagaci* expression1 = parse(tokens1, degisken1);
            double sonuc1 = calculate(expression1, degisken1);

            char* tokens2 = tokenize(in);
            hesaplamaagaci* expression2 = parse(tokens2, degisken2);
            double sonuc2 = calculate(expression2, degisken2);

            char* tokens3 = tokenize(in);
            hesaplamaagaci* expression3 = parse(tokens3, degisken3);
            double sonuc3 = calculate(expression3, degisken3);

            char* tokens4 = tokenize(in);
            hesaplamaagaci* expression4 = parse(tokens4, degisken4);
            double sonuc4 = calculate(expression4, degisken4);

            printf("%lf-%lf-%lf-%lf : %lf-%lf-%lf-%lf\n", degisken1, degisken2, degisken3, degisken4, sonuc1 , sonuc2 , sonuc3 , sonuc4);
            result += sonuc1 + 3 * sonuc2 + 3 * sonuc3 + sonuc4;// y0+3y1+3y2+y3

            free_hesaplamaagaci(expression1);
            free_hesaplamaagaci(expression2);
            free_hesaplamaagaci(expression3);
            free_hesaplamaagaci(expression4);
            free(tokens1);
            free(tokens2);
            free(tokens3);
            free(tokens4);
        }
        result *= 3 * step / 8;
    } else {
        fprintf(stderr, "Invalid method\n");// yanl bir inputa kar
        exit(1);
    }

    return result;
}}
/*
@brief Bisection yontemi ile [a,b] araliginda kok arar.
@param degisken1 Parametre.
@param degisken2 Parametre.
@param hatapayi Parametre.
@param in Parametre.
@return Kok yaklasimi (double).
*/
double bisection(double degisken1, double degisken2, double hatapayi, char* in) {
    double sonuc1, sonuc2, ortalama, sonucOrtalama;
    char* tokens1 = tokenize(in);
    hesaplamaagaci* expression1 = parse(tokens1, degisken1);
    sonuc1 = calculate(expression1, degisken1);
    free_hesaplamaagaci(expression1);

    char* tokens2 = tokenize(in);
    hesaplamaagaci* expression2 = parse(tokens2, degisken2);
    sonuc2 = calculate(expression2, degisken2);
    free_hesaplamaagaci(expression2);

    if (sonuc1 * sonuc2 > 0) {
        printf("Bu saylar bisection yntemi iin uygun deil\n");// bisectionda kk bulabilmek iin fx0 ve fx1 in iaretleri farkl olmal
        exit(1);
    } else if (sonuc1 * sonuc2 == 0) {// eer ki ikisinin arpm 0 sa  ikisinin x deerinden en az biri kktr
        if (sonuc1 == 0) {
            return degisken1;
        } else {
            return degisken2;
        }
    } else {
        int iterasyon = 1;
        int k = 1;
        while (fabs((degisken2 - degisken1) / k) > hatapayi) {
            ortalama = (degisken1 + degisken2) / 2.0;
            k*=2;
            char* tokensOrtalama = tokenize(in);
            hesaplamaagaci* expressionOrtalama = parse(tokensOrtalama, ortalama);
            sonucOrtalama = calculate(expressionOrtalama, ortalama);
            free_hesaplamaagaci(expressionOrtalama);

            printf("%d. iterasyon: %lf\n", iterasyon, ortalama);
            iterasyon++;

            if (sonucOrtalama == 0) {
                return ortalama;
            } else if (sonuc1 * sonucOrtalama < 0) {
                degisken2 = ortalama;
                sonuc2 = sonucOrtalama;
            } else {
                degisken1 = ortalama;
                sonuc1 = sonucOrtalama;
            }
        }
        return (degisken1 + degisken2) / 2.0;
    }
}
/*
@brief Regula-Falsi (False Position) yontemi ile [a,b] araliginda kok arar.
@param degisken1 Parametre.
@param degisken2 Parametre.
@param hatapayi Parametre.
@param in Parametre.
@return Kok yaklasimi (double).
*/
double regulafalsi(double degisken1, double degisken2, double hatapayi, char* in) {
       double sonuc1, sonuc2, kiris, sonuckiris;
    char* tokens1 = tokenize(in);
    hesaplamaagaci* expression1 = parse(tokens1, degisken1);
    sonuc1 = calculate(expression1, degisken1);
    free_hesaplamaagaci(expression1);

    char* tokens2 = tokenize(in);
    hesaplamaagaci* expression2 = parse(tokens2, degisken2);
    sonuc2 = calculate(expression2, degisken2);
    free_hesaplamaagaci(expression2);

    if (sonuc1 * sonuc2 > 0) {
        printf("Bu saylar regulafalsi yntemi iin uygun deil\n");//regulafalside kk bulabilmek iin fx0 ve fx1 in iaretleri farkl olmal
        exit(1);
    } else if (sonuc1 * sonuc2 == 0) {
        if (sonuc1 == 0) {
            return degisken1;
        } else {
            return degisken2;
        }}
        else {
        int iterasyon = 1;
        int k = 1;
        while (fabs((degisken2 - degisken1) / k) > hatapayi) {
              kiris = (degisken2 * sonuc1 - degisken1 * sonuc2) / (sonuc1 - sonuc2); // slayttaki forml uyguladm
              k*=2;
              char* tokenskiris = tokenize(in);
            hesaplamaagaci* expressionkiris = parse(tokenskiris, kiris);
            sonuckiris = calculate(expressionkiris, kiris);
            free_hesaplamaagaci(expressionkiris);
            printf("%d. iterasyon: %lf\n", iterasyon, kiris);
            iterasyon++;
            if (sonuckiris == 0) {
                return kiris;
            } else if (sonuc1 * sonuckiris < 0) {
                degisken2 = kiris;
                sonuc2 = sonuckiris;
            } else {
                degisken1 = kiris;
                sonuc1 = sonuckiris;
            }}}
}
/*
@brief Newton-Raphson yontemi ile tek degiskenli kok yaklasimi yapar; iterasyonlari yazdirir.
@param degisken Parametre.
@param hatapayi Parametre.
@param in Parametre.
@return Kok yaklasimi (double).
*/
double newtonraphson(double degisken, double hatapayi, char *in) {
    double x = degisken;
    double prev_x = x;
    int iterasyon = 1;
    
    while (iterasyon <= 10) { // raksamay 10 iterasyonla ltm nk ok hzl yaknlaan bir yntem 10 iterasyonda bulamyorsa ok ok yksek ihtimalle raksyordur
        char* tokens = tokenize(in);
        hesaplamaagaci* expression = parse(tokens, x);
        double f = calculate(expression, x);
        free_hesaplamaagaci(expression);
        double f_derivative = sayisalturev(x, 0.000001,'m', in);
        double x_next = x - (f / f_derivative);
        if (fabs(x_next - prev_x) <= hatapayi) { 
            return prev_x;
        }
        printf("%d. iterasyon: x = %lf, f(x) = %lf\n", iterasyon, x, f);
        prev_x = x;
        x = x_next;
        iterasyon++;
    }
    printf("bu verdiiniz kk raksamaktadr");
    return x;
}     
/*
@brief Duzenli aralikli veri icin Gregory-Newton ileri fark enterpolasyonu uygular ve polinomu yazdirir.
@param h Parametre.
@param variable Parametre.
@param elemansayisi Parametre.
@param sonuckumesi[elemansayisi] Parametre.
@param ilkx Parametre.
@return Enterpolasyon sonucu (double).
*/
double gregorynewtonenterpolasyonu(double h, double variable, int elemansayisi, double sonuckumesi[elemansayisi], double ilkx)
{
       double **ilerifarktablosu = (double**)malloc(elemansayisi * sizeof(double*));
       int i,j;
       for (i = 0; i < elemansayisi; i++) {
       ilerifarktablosu[i] = (double *)malloc((elemansayisi-i) * sizeof(double));}// byle tanmlamamn sebebi bu alann bana yetiyor olmas
       for (i = 0; i < elemansayisi; i++)  {
           ilerifarktablosu[0][i] = sonuckumesi[i];}
       for (i = 1; i < elemansayisi; i++)  {
           for (j = 0; j < elemansayisi - i; j++)  {
               ilerifarktablosu[i][j] = ilerifarktablosu[i-1][j+1] - ilerifarktablosu[i-1][j];}}// ileri fark tablosu oluturma
       printf("Ileri Farklar Tablosu:\n");
       for (i = 0; i < elemansayisi; i++) {
        for (j = 0; j < elemansayisi - i; j++) {
            printf("%15.5f ", ilerifarktablosu[j][i]);
        }
        printf("\n");
    }
       double sum = ilerifarktablosu[0][0];
       double division = 1;
       for(i = 1; i < elemansayisi; i++)  {
             double multiple = 1;
             division *= (i*h);
             for (j = 0; j < i; j++) {// polinomu yazdrmak iin
                 multiple *= (variable - ilkx - j * h);
                 if (j > 0) printf(" * ");
                 printf("(x - %.5f)", ilkx + j * h);}
                 printf("* %lf ",ilerifarktablosu[i][0]);
                 printf("/ %lf ", division);
                 if(i != elemansayisi -1)
                 {printf(" + ");}
             sum += multiple * ilerifarktablosu[i][0] / division ;}// ileri fark tablosu ayn olana kadar devam edilir normalde ama ztn byle bir durumda sonraki elemanlarn hepsi 0 olaca iin kontrole gerek olmadn dndm
             for (i = 0; i < elemansayisi; i++) {
        free(ilerifarktablosu[i]);
    }
    free(ilerifarktablosu);
       return sum;
}
/*
@brief Gauss-Jordan eliminasyonu ile NxN matrisin tersini hesaplar.
@param N Parametre.
@param matris Parametre.
@param tersmatris Parametre.
@return Deger dondurmez; sonuc tersmatris'e yazilir.
*/
void matrisintersi(int N, double **matris, double **tersmatris) {
    int i, j, k;
    double temp;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            if (i == j)
                tersmatris[i][j] = 1.0;//birim matris oluturma
            else
                tersmatris[i][j] = 0.0;
        }
    }
    for (i = 0; i < N; i++) {
        if (matris[i][i] == 0) {
            int pivot_row = -1;
            j = i + 1;
            while (j < N && pivot_row == -1) {
            if (matris[j][i] != 0) {
            pivot_row = j;//pivot elemanlardan biri 0 ise deitirmek lazm yoksa hata verir
            }
            j++;
            }
            if (pivot_row == -1) {
                printf("Bu matrisin tersi alnamaz!\n");// eer ki hepsi 0 sa determinant 0 demektir bu da matrisin singler olduu ve terslenemedii anlamna gelir
                return;
            }
            for (k = 0; k < N; k++) {
                temp = matris[i][k];
                matris[i][k] = matris[pivot_row][k];
                matris[pivot_row][k] = temp;
                temp = tersmatris[i][k];
                tersmatris[i][k] = tersmatris[pivot_row][k];//matrisin satrlarn deitirme ilemi
                tersmatris[pivot_row][k] = temp;
            }
        }
        double pivot = matris[i][i];
        for (j = 0; j < N; j++) {
            matris[i][j] /= pivot;
            tersmatris[i][j] /= pivot;//pivotlar 1 yaptk ve hepsini bu orana gre bldk
        }
        for (j = 0; j < N; j++) {
            if (j != i) {
                double multiplier = matris[j][i];
                for (k = 0; k < N; k++) {
                    matris[j][k] -= multiplier * matris[i][k];// pivot elemann hem stndeki hem altndaki elemanlar 0 ladk bu sayede bu dngnn sonunda asl matrisimiz birim matris olarak kalacak
                    tersmatris[j][k] -= multiplier * tersmatris[i][k];
                }
            }
        }
    }
}
/*
@brief Katsayi ve sonuc girisi icin denklem sablonunu yazar.
@param N Parametre.
@return Deger dondurmez.
*/
void printEquationTemplate(int N) {
    int i, j;
    for (i = 1; i <= N; i++) {//gzel bir grsel olsun diye denklem zme yntemlerinde byle bir ema kullandm hem daha rahat bir ekilde inputlar girmek mmkn oluyor
        for (j = 1; j <= N; j++) {
            printf("%c%d*x%d", 'a' + (j - 1), i, j);
            if (j < N) {
                printf(" + ");
            }
        }
        printf(" = %c%d*y\n", 'a' + N, i);
    }
}
/*
@brief Girilen lineer denklem sistemini okunur bicimde yazar.
@param N Parametre.
@param katsayilarmatrisi Parametre.
@param sonucmatrisi Parametre.
@return Deger dondurmez.
*/
void printequationsystem(int N, double **katsayilarmatrisi, double *sonucmatrisi) {
    printf("\nGirilen denklemler:\n");
    int i, j;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            printf("%.6lf*x%d", katsayilarmatrisi[i][j], j + 1);
            if (j < N - 1) {
                printf(" + ");
            }
        }
        printf(" = %.6lf*y\n", sonucmatrisi[i]);
    }
}
/*
@brief Gauss eliminasyonu ve geri yerine koyma ile lineer sistemi cozer.
@param N Parametre.
@param katsayilarmatrisi Parametre.
@param sonucmatrisi Parametre.
@param degiskenmatrisi Parametre.
@return Deger dondurmez; sonuc degiskenmatrisi'ne yazilir.
*/
void gausseleminasyon(int N, double **katsayilarmatrisi, double *sonucmatrisi, double *degiskenmatrisi) {
    int i, j, k;
    double temp;
    for (i = 0; i < N; i++) {
        if (katsayilarmatrisi[i][i] == 0) {
            int pivot_row = -1;
            j = i + 1;
            while (j < N && pivot_row == -1) {
            if (katsayilarmatrisi[j][i] != 0) {
            pivot_row = j;
            }
            j++;
            }
            if (pivot_row == -1) {//pivot elemanlardan biri 0 ise deitirmek lazm yoksa hata verir
                printf("Sonsuz zm veya zm yok.\n");//matrisin determinant 0 olduunda ya sonsuz zm vardr ya da zm yoktur
                return;
            } else {
                for (k = 0; k < N; k++) {
                    temp = katsayilarmatrisi[i][k];
                    katsayilarmatrisi[i][k] = katsayilarmatrisi[pivot_row][k];
                    katsayilarmatrisi[pivot_row][k] = temp;
                }
                temp = sonucmatrisi[i];
                sonucmatrisi[i] = sonucmatrisi[pivot_row];//matrisin satrlarn deitirme ilemi
                sonucmatrisi[pivot_row] = temp;
            }
        }
        double pivot = katsayilarmatrisi[i][i];
        for (j = i; j < N; j++) {
            katsayilarmatrisi[i][j] /= pivot;// pivot ve sonrasndaki elemanlar pivota blyoruz 
        }
        sonucmatrisi[i] /= pivot;

        for (j = i + 1; j < N; j++) {
            double factor = katsayilarmatrisi[j][i];
            for (k = i; k < N; k++) {
                katsayilarmatrisi[j][k] -= factor * katsayilarmatrisi[i][k];// bu ilemle de st gen matris haline getiriyoruz
            }
            sonucmatrisi[j] -= factor * sonucmatrisi[i];
        }
    }

    for (i = N - 1; i >= 0; i--) {
        degiskenmatrisi[i] = sonucmatrisi[i];
        for (j = i + 1; j < N; j++) {
            degiskenmatrisi[i] -= katsayilarmatrisi[i][j] * degiskenmatrisi[j];// bu ilem sayesinde de alttan ste doru tarayp ii biten deikenin stununu 0 ladmz iin btn deikenlerin deeri sonu matrisinde kalm oluyor
        }
    }
}

/*
@brief NxN matrisi formatli olarak yazar.
@param N Parametre.
@param matris Parametre.
@return Deger dondurmez.
*/
void printMatrix(int N, double **matris) {
    int i,j;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            printf("%10.4f ", matris[i][j]);
        }
        printf("\n");
    }
} 
/*
@brief Gauss-Seidel iteratif yontemi ile yaklasik cozum uretir; her iterasyonu yazar.
@param N Parametre.
@param katsayilarmatrisi Parametre.
@param sonucmatrisi Parametre.
@param degiskenmatrisi Parametre.
@param iterasyonsayisi Parametre.
@return Deger dondurmez; degerler degiskenmatrisi'ne yazilir.
*/
void gaussseidel(int N, double **katsayilarmatrisi, double *sonucmatrisi, double *degiskenmatrisi, int iterasyonsayisi)
{
     int i, j, k;
    double *xiterated = (double*)malloc(N * sizeof(double));
    for (i = 0; i < N; i++) {
        xiterated[i] = degiskenmatrisi[i];}
    for (i = 0; i < N; i++) {
        int max_row = i;
        for (k = i + 1; k < N; k++) {
            if (fabs(katsayilarmatrisi[k][i]) > fabs(katsayilarmatrisi[max_row][i])) {
                max_row = k; // kegensel dominant olan matrislerde gauss seidel daha doru alr o yzden imkanmz varsa kegensel dominant hale getiriyoruz
            }
        }
        if (max_row != i) {
            double temp;
            for (j = 0; j < N; j++) {
                temp = katsayilarmatrisi[i][j];
                katsayilarmatrisi[i][j] = katsayilarmatrisi[max_row][j];//atrisin satrlarn deitirme ilemi
                katsayilarmatrisi[max_row][j] = temp;
            }
            temp = sonucmatrisi[i];
            sonucmatrisi[i] = sonucmatrisi[max_row];
            sonucmatrisi[max_row] = temp;
        }
    }
    for (k = 0; k < iterasyonsayisi; k++) {
        for (i = 0; i < N; i++) {
            double sumiterated = 0.0;
            for (j = 0; j < i; j++) {
                sumiterated += katsayilarmatrisi[i][j] * xiterated[j];// her x deeri sonucu belirlendii anda gncellendii iin iterated ve noniterated olarak 2 paraya ayrdm biri nceki iterasyondaki paralar tutuyor dieri de yeni itere edilmi deerleri tutuyor
            }// bu sayede nceki deerlerin iterasyondan gemi x lerle topluyorum sonrakiler daha itere edilmemi olduu iin onlarn nceki iterasyondaki deerlerini kullanyorum
            double sumnoniterated = 0.0;
            for (j = i + 1; j < N; j++) {
                sumnoniterated += katsayilarmatrisi[i][j] * degiskenmatrisi[j];
            }
            xiterated[i] = (sonucmatrisi[i] - sumiterated - sumnoniterated) / katsayilarmatrisi[i][i];// bu da denklemde deikeni yalnz brakmak iin
        }

        for (i = 0; i < N; i++) {
            degiskenmatrisi[i] = xiterated[i];// bu btn x deerleri iterasyondan getikten sonra bir sonraki iterasyon iin eski x deerlerini gncelliyor
        }

        printf("%d. iterasyon\n", k + 1);
        for (i = 0; i < N; i++) {
            printf("x%d: %lf\n", i + 1, degiskenmatrisi[i]);
        }
    }

    free(xiterated);
}   
/*
@brief Program girisi; menuyu gosterir ve secilen sayisal yontemi calistirir.
@param argc Parametre.
@param argv[] Parametre.
@return Basariyla tamamlaninca 0.
*/
int main(int argc, char* argv[]) {
    while (1) {
        char in[MAX_INPUT_SIZE];
        int choice;
        double variable;

        printf("1. Bisection\n");
        printf("2. Regula-Falsi\n");
        printf("3. Newton-Raphson\n");
        printf("4. Matrisin Tersi\n");
        printf("5. Gauss Eleminasyon\n");
        printf("6. Gauss Seidal\n");
        printf("7. Sayisal Turev\n");
        printf("8. Trapez\n");
        printf("9. Simpson\n");
        printf("10. Gregory Newton Enterpolasyonu\n");
        printf("11. Hesap Makinesi\n");
        printf("What is your choice? ");
        scanf("%d", &choice);

        if (choice == 11) {
            printf("Enter input: ");
            scanf(" %[^\n]", in);
            printf("Enter the value for x: ");
            scanf("%lf", &variable);
            char* tokens = tokenize(in);//ifadeyi paralyor
            hesaplamaagaci* expression = parse(tokens, variable);//ifadeyi parse ediyor
            double value = calculate(expression, variable);//ifadeyi hesaplyor
            printf("The result is: %lf\n", value);
            free_hesaplamaagaci(expression);
            free(tokens);
        } 
        else if(choice == 1)
        {
             double degisken1,degisken2,hatapayi;
             printf("Enter input: ");
             scanf(" %[^\n]", in);
             printf("Enter smaller variable: ");
             scanf("%lf",&degisken1);
             printf("Enter bigger variable: ");
             scanf("%lf",&degisken2);
             printf("Enter tolerance: ");
             scanf("%lf",&hatapayi);
             double result = bisection(degisken1,degisken2,hatapayi,in);
             printf("______________________________________\n");
        }
        else if(choice == 2)
        {
             double degisken1,degisken2,hatapayi;
             printf("Enter input: ");
             scanf(" %[^\n]", in);
             printf("Enter smaller variable: ");
             scanf("%lf",&degisken1);
             printf("Enter bigger variable: ");
             scanf("%lf",&degisken2);
             printf("Enter tolerance: ");
             scanf("%lf",&hatapayi);
             double result = regulafalsi(degisken1,degisken2,hatapayi,in);
             printf("______________________________________\n");
        }
        else if(choice == 3)
        {
             double degisken,hatapayi;
             printf("Enter input: ");
             scanf(" %[^\n]", in);
             printf("Enter variable: ");
             scanf("%lf",&degisken);
             printf("Enter tolerance: ");
             scanf("%lf",&hatapayi);
             double result = newtonraphson(degisken,hatapayi,in);
             printf("______________________________________\n");
        }
        else if(choice == 4)
{
    int N;
    int i,j;
    printf("Enter the dimension: ");
    scanf("%d", &N);
    double **matris = (double**)malloc(N * sizeof(double*));
    double **tersmatris = (double**)malloc(N * sizeof(double*));
    for (i = 0; i < N; i++) {
        matris[i] = (double*)malloc(N * sizeof(double));
        tersmatris[i] = (double*)malloc(N * sizeof(double));
    }
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            printf("Enter the matrix's [%d,%d] data: ", i + 1, j + 1);
            scanf("%lf", &matris[i][j]);
        }
    }
    printf("Baslangic Matrisi:\n");
    printMatrix(N, matris);
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            tersmatris[i][j] = (i == j) ? 1 : 0;
        }
    }
    matrisintersi(N,matris,tersmatris);
    printf("\nTers Matris:\n");
    printMatrix(N, tersmatris);
}
else if(choice == 5)
{
     int N;
     int i,j;
     printf("Enter how many variables there are: ");
     scanf("%d", &N);
     double **katsayilarmatrisi = (double**)malloc(N * sizeof(double*));
     for (i = 0; i < N; i++) {
        katsayilarmatrisi[i] = (double*)malloc(N * sizeof(double));}
     double *sonucmatrisi = (double*)malloc(N * sizeof(double));
     double *degiskenmatrisi = (double*)malloc(N * sizeof(double));
     printEquationTemplate(N);
     for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            printf("%c%d: ", 'a' + j, i + 1);
            scanf("%lf", &katsayilarmatrisi[i][j]);
        }
        printf("%c%d: ", 'a' + N, i + 1);
        scanf("%lf", &sonucmatrisi[i]);
    }
    printequationsystem(N, katsayilarmatrisi, sonucmatrisi);
    gausseleminasyon(N, katsayilarmatrisi, sonucmatrisi, degiskenmatrisi);
    for(i = 0; i < N; i++)
    {
          printf("x%d: %lf\n ",i+1,degiskenmatrisi[i]);
    }   
}
else if(choice == 6)
{
     int N;
        int i, j, iterasyonsayisi;
        printf("Enter how many variables there are: ");
        scanf("%d", &N);
        double **katsayilarmatrisi = (double**)malloc(N * sizeof(double*));
        for (i = 0; i < N; i++) {
            katsayilarmatrisi[i] = (double*)malloc(N * sizeof(double));
        }
        double *sonucmatrisi = (double*)malloc(N * sizeof(double));
        double *degiskenmatrisi = (double*)malloc(N * sizeof(double));
        printEquationTemplate(N);
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                printf("%c%d: ", 'a' + j, i + 1);
                scanf("%lf", &katsayilarmatrisi[i][j]);
            }
            printf("%c%d: ", 'a' + N, i + 1);
            scanf("%lf", &sonucmatrisi[i]);
        }
        printequationsystem(N, katsayilarmatrisi, sonucmatrisi);

        for (i = 0; i < N; i++) {
            printf("%c%d: ", 'x', i + 1);
            scanf("%lf", &degiskenmatrisi[i]);
        }
        printf("Enter the iteration time: ");
        scanf("%d", &iterasyonsayisi);
        gaussseidel(N, katsayilarmatrisi, sonucmatrisi, degiskenmatrisi, iterasyonsayisi);
        for (i = 0; i < N; i++) {
            free(katsayilarmatrisi[i]);
        }
        free(katsayilarmatrisi);
        free(sonucmatrisi);
        free(degiskenmatrisi); 
}
        else if (choice == 7) {
            printf("Enter input: ");
            scanf(" %[^\n]", in);
            printf("Enter the value for x: ");
            scanf("%lf", &variable);
            printf("[i] Ileri fark\n");
            printf("[g] Geri fark\n");
            printf("[m] Merkezi fark\n");
            char igm;
            printf("What is your choice? ");
            scanf(" %c", &igm);
            printf("Enter the value for h: ");
            double h;
            scanf("%lf", &h);

            double result = sayisalturev(variable, h, igm, in);
            if (igm == 'i')
                printf("process: (f(%lf)-f(%lf))/%lf\n  result: %lf\n", variable + h, variable, h, result);
            else if (igm == 'g')
                printf("process: (f(%lf)-f(%lf))/%lf\n  result: %lf\n", variable, variable - h, h, result);
            else if (igm == 'm')
                printf("process: (f(%lf)-f(%lf))/%lf\n  result: %lf\n", variable + h, variable - h, 2 * h, result);
        } else if (choice == 8) {
            double limit1, limit2;
            unsigned int n;
            printf("Enter input: ");
            scanf(" %[^\n]", in);
            printf("Enter the limits of integral:\n");
            printf("smaller limit: ");
            scanf("%lf", &limit1);
            printf("bigger limit: ");
            scanf("%lf", &limit2);
            printf("Number of pieces: ");
            scanf("%u", &n);
            double result = trapez(limit1, limit2, n, in);
            printf("result: %lf\n", result);
        } else if (choice == 9) {
            double limit1, limit2;
            unsigned int n;
            char decider;
            printf("Enter input: ");
            scanf(" %[^\n]", in);
            printf("Enter the limits of integral:\n");
            printf("smaller limit: ");
            scanf("%lf", &limit1);
            printf("bigger limit: ");
            scanf("%lf", &limit2);
            printf("Number of pieces: ");
            scanf("%u", &n);
            printf("[o] Simpson 1/3\n");
            printf("[l] Simpson 3/8\n");
            printf("What is your choice? ");
            scanf(" %c", &decider);

            double result = simpson(limit1, limit2, n, in, decider);
            printf("result: %lf\n", result);
        }
          else if (choice == 10)
          {
               double variable, h, ilkx;
               int i, elemansayisi;
               printf("How many elements will you enter: ");
               scanf("%d", &elemansayisi);
               printf("Enter the smallest data : ");
               scanf("%lf", &ilkx);
               printf("Enter the increase amount: ");
               scanf("%lf", &h);
               printf("Enter the value for x: ");
               scanf("%lf", &variable);
               double *sonuckumesi = (double *)malloc(elemansayisi * sizeof(double));
               for (i = 0; i < elemansayisi; i++) {
                   printf("%d. elemanin sonucunu giriniz: ", i + 1);
                   scanf("%lf", &sonuckumesi[i]);
                   }

                   double result = gregorynewtonenterpolasyonu(h, variable, elemansayisi, sonuckumesi, ilkx);
                   printf("\nThe result is: %lf\n", result);
                   free(sonuckumesi);    
          }
    }
    return 0;
}

            

                   

