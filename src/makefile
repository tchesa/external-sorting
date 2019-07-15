#Makefile para o TP2
CFILES := main.c heapsort.c intercalacaobalanceada.c lista.c quicksort.c quicksort_externo.c
PROG := ordena
CFLAGS := -O3 -g
LDFLAGS :=
########################
CFLAGS += -MMD
CC := g++ -std=c++11

OBJFILES := $(CFILES:.c=.o)
DEPFILES := $(CFILES:.c=.d)
GCHFILES := $(CFILES:.c=.gch)

$(PROG) : $(OBJFILES)
		$(LINK.o) $(LDFLAGS) -o $@ $^

clean :
		rm -f $(PROG) $(OBJFILES) $(DEPFILES)

-include $(DEPFILES)
