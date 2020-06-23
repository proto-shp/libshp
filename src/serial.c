#include "libshp/serial.h"

#include <string.h>
#include <stdio.h>

#include <stdarg.h>

int eat_spaces(const char *pattern, int *index){
	while(pattern[*index] == ' ') (*index)++;

	return 1;
}

int parse_ident(const char *pattern, char *result, int max, int *index){
	char c = pattern[*index];
	if(!(ALPHA(c))){
		printf("An identifier should begin with an alpha character\n");
		return 0;
	}
	int i = 0;
	while(ALPHA(c) || (c >= '0' && c <= '9') || c== '_'){
		result[i++] = c;
		(*index)++;
		if(*index >= max){
			printf("Ran out of buffer space for parse_ident\n");
			return 0;
		}
		c = pattern[*index];
		if(c == '\0') break;
	}
	result[i] = '\0';

	return 1;
}

int type_from_ident(const char *ident){
	if(strcmp(ident, "u8") == 0){
		return U8;
	}else if(strcmp(ident, "s8") == 0){
		return S8;
	}else if(strcmp(ident, "u16") == 0){
		return U16;
	}else if(strcmp(ident, "s16") == 0){
		return S16;
	}else if(strcmp(ident, "u32") == 0){
		return U32;
	}else if(strcmp(ident, "s32") == 0){
		return S32;
	}else if(strcmp(ident, "string") == 0){
		return STRING;
	}else{
		return -1;
	}
}

int parse_type(const char* pattern, int *index, Type *type){
	char buffer[256];
	eat_spaces(pattern, index);
	if(!parse_ident(pattern, buffer, 256, index)){
		printf("Failed to parse, type %s is not a valid identifier\n", buffer);
		return 0;
	}
	int t = type_from_ident(buffer);
	if(t == -1){
		printf("Unknow type '%s'\n", buffer);
		return 0;
	}
	type->type = t;
	type->is_array = 0;

	return 1;
}

int next_type(const char *pattern, int *index, Type *type){
	eat_spaces(pattern, index);
	if(pattern[*index] == ')') return 0;
	if(pattern[*index] == '(' || pattern[*index] == ','){
		(*index)++;
	}
	parse_type(pattern, index, type);

	return 1;
}

int write_message_payload(const char *format, Message_Payload *message, ...){
	va_list argp;
	va_start(argp, message);
	
	Type t;
	int index = 0;
	while(next_type(format, &index, &t)){
		switch(t.type){
			case U8:
				add_u8(message, va_arg(argp, int));
				break;
			case S8:
				add_s8(message, va_arg(argp, int));
				break;
			case U16:
				add_u16(message, va_arg(argp, int));
				break;
			case S16:
				add_s16(message, va_arg(argp, int));
				break;
			case U32:
				add_u32(message, va_arg(argp, int));
				break;
			case S32:
				add_s32(message, va_arg(argp, int));
				break;
			case STRING:
				printf("Add string\n");
				break;
		}
	}

	va_end(argp);

	return 1;
}

int read_message_payload(const char *format, Message_Payload *message, ...){
	va_list argp;
	va_start(argp, message);

	void *data = message->arguments;

	Type t;
	int index = 0;
	while(next_type(format, &index, &t)){
		switch(t.type){
			case U8:
				*va_arg(argp, int*) = get_u8(&data);
				break;
			case S8:
				*va_arg(argp, int*) = get_s8(&data);
				break;
			case U16:
				*va_arg(argp, int*) = get_u16(&data);
				break;
			case S16:
				*va_arg(argp, int*) = get_s16(&data);
				break;
			case U32:
				*va_arg(argp, int*) = get_u32(&data);
				break;
			case S32:
				*va_arg(argp, int*) = get_s32(&data);
				break;
			case STRING:
				printf("get string\n");
				break;
		}
	}

	va_end(argp);

	return 1;
}

