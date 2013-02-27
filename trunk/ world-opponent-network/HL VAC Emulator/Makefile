.PHONY: all clean

MAIN=vacemu

all: $(MAIN)

clean:
	$(RM) *.o *.class $(MAIN) $(MAIN).jar


$(MAIN): $(patsubst %.java,%.o,$(wildcard *.java))

$(MAIN).jar: $(MAIN).class


%: %.o
	gcj --main=$@ $^ -o $@

%.o: %.java
	gcj -c $<

%.jar: %.class
	jar cfm $@ MANIFEST.MF $(patsubst %.java,%.class,$(wildcard *.java))

$(MAIN).class: $(wildcard *.java)
	javac $^
