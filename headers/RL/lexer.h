#ifndef _RL_LEXER_H
#define _RL_LEXER_H

#include "../types.h"

#include "../ugsm.h"

enum RLTokenType {
	RLNUMBER,
	
	RLWORD,
	
	// keyword
	
	RETURN,

	// parentheses

	LPAR, // (

	RPAR, // )

	LBRACE, // {

	RBRACE, // }

	RLCOMMA, // ,

	RLEOF // end of file
};

struct RLToken {
	enum RLTokenType type;

	UGSMGlyphCode word[12];
};

void RLTokenize(UGSMGlyphCode code[384]);

void RLTokensView(void);

#endif