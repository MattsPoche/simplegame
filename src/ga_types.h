#ifndef GA_TYPES_H_
#define GA_TYPES_H_

#define ENTITY_TAG_LEN 24
typedef struct {
	int upbutton_state;
	int downbutton_state;
	int leftbutton_state;
	int rightbutton_state;
	int floatbutton_state;
	int sinkbutton_state;
} Control_State;

typedef struct {
	float frequency;
} Audio_State;

typedef struct {
	int active;
	Vec3 pos;
	float scalar;
	uint32_t color;
	Model_Data *model;
	char tag[ENTITY_TAG_LEN];
	int tag_len;
} Entity;

typedef struct {
	Vec3 pos;
	float scalar;
	uint32_t color;
	size_t m;		/* index of model in model array */
	char *tag;
} Entity_Def;

#endif /* GA_TYPES_H_ */
