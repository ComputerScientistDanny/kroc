#include "tvm-scc.h"

static tvm_t tvm;
tvm_ectx_t context;

#define MEM_WORDS 51200
static WORD raw_memory[MEM_WORDS + 1];
static WORDPTR memory;

/* The bytecode file, loaded into flash at a fixed address. */
extern const unsigned char transputerbytecode[];
static const BYTE *tbc_data = transputerbytecode;
static BYTEPTR initial_iptr;

static WORD get_time(ECTX ectx) {
	return time_millis();
}

static void modify_sync_flags(ECTX ectx, WORD set, WORD clear) {
	ectx->sflags = (ectx->sflags & (~clear)) | set;
}

int main () {
	int i;
#ifdef PREREAD
	WORD tmpWORD;
	BYTE tmpBYTE;
#endif

	enable();

	/* The Transputer memory must be word-aligned. */
	memory = (WORDPTR) (((int) (raw_memory + 1)) & ~1);

	for (i = 0; i < MEM_WORDS; i++) {
		memory[i] = MIN_INT;
//Preread the memory pool
#ifdef PREREAD
		tmpWORD = memory[i];
#endif
	}

	tvm_init (&tvm);
	tvm_ectx_init (&tvm, &context);

	if (init_context_from_tbc(&context, tbc_data, memory, MEM_WORDS) != 0) {
		printf("ERRO: init_context_from_tbc() failed.\n");
	}
	initial_iptr = context.iptr;

//Preread the bytecode
#ifdef PREREAD
	for(i = 0; i < sizeof(transputerbytecode); i++)
		tmpBYTE = transputerbytecode[i];
#endif

	context.get_time = get_time;
	context.modify_sync_flags = modify_sync_flags;
	context.sffi_table = sffi_table;
	context.sffi_table_length = sffi_table_length;

	printf("INFO: SFFI table length is %d.\n", sffi_table_length);
	
	printf("INFO: Starting TVM ...\n");

	while(1){}

	while (1) {
		int ret = tvm_run (&context);

		switch (ret) {
			case ECTX_PREEMPT:
			case ECTX_TIME_SLICE: {
				break;
			}
			case ECTX_SLEEP: {
				WORD next = context.tnext;
				WORD now;
				do {
					now = time_millis();
				} while (TIME_AFTER (next, now));
				break;
			}
			case ECTX_INTERRUPT: {
				break;
			}
			case ECTX_EMPTY: {
				break;
			}
			case ECTX_SHUTDOWN: {
				printf("INFO: End of program reached.\n");
			}
			default: {
				printf("ERRO: Exit status is %x.\n", &ret);
			}
		}
	}

	/* NOTREACHED */
	return 0;
}
