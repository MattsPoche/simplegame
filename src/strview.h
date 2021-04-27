#ifndef STR_VIEW_H_
#define STR_VIEW_H_

typedef struct {
	char *str;
	int len;
} Str_View;

typedef struct {
	Str_View sv;
	Str_View rest;
} Str_Str;

#endif /* STR_VIEW_H_ */

#ifdef STR_VIEW_IMPLEMENTATION

Str_Str
sv_word(Str_View sv)
{
	Str_View word = {
		.str = sv.str,
		.len = 0,
	};
	while ((isalnum(*sv.str) || *sv.str == '_') && sv.len > 0) {
		sv.str++;
		sv.len--;
		word.len++;
	}

	return (Str_Str){ word, sv };
}

Str_View
sv_skip_whitespace(Str_View sv)
{
	for (; isspace(*sv.str) && sv.len > 0; ++sv.str, --sv.len);
	return sv;
}

Str_View
sv_skip_to_next_ln(Str_View sv)
{
	for (; *sv.str != '\n' && sv.len > 0; ++sv.str, --sv.len);
	++sv.str;
	--sv.len;
	return sv;
}
#endif /* STR_VIEW_IMPLEMENTATION */
