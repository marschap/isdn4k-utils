# Makefile fuer i4l-faq
# (c) 1997 by Matthias Heßler (MatHes)
#
# Erzeugt FAQ etc.

# Directories
# BIN enthaelt alle Perl-Skripte
BIN = ./bin
# in OUT werden alle Output-Dateien erzeugt
GEN_DIR = ./out
# Für Bernhard:
TARGET_DIR = /home/dl4mhk/www/i4l-faq
# Ich selbst behalte den Kram lieber da, wo er erzeugt wurde:
#TARGET_DIR = $(GEN_DIR)

# Programm-Namen
HTML = $(BIN)/html.pl
7BIT = $(BIN)/7bit.pl
OLD = $(BIN)/old.pl
NEW = $(BIN)/new.pl
ZIP = zip
GZIP = gzip

# Quell-Dateien
SRC = de-i4l-faq
OLD_SRC = $(SRC).old
SRC_DIFF = $(SRC).diff

SRC_ENG = eng-i4l-faq
OLD_SRC_ENG = $(SRC_ENG).old
SRC_ENG_DIFF = $(SRC_ENG).diff

# Ausgabe-Dateien
OUT_7BIT = $(SRC).asc
OUT_HTML = $(SRC).html
OUT_ENG_7BIT = $(SRC_ENG).asc
OUT_ENG_HTML = $(SRC_ENG).html


# Aufrufe zur Erzeugung verschiedener FAQ-Versionen:
# alles
all:	7bit html e7bit ehtml \
	7bit.zip html.zip e7bit.zip ehtml.zip \
	7bit.gz html.gz e7bit.gz ehtml.gz install

# deutsche 7Bit-Version
7bit: $(GEN_DIR)/$(OUT_7BIT)
7bit.zip: $(GEN_DIR)/$(OUT_7BIT).zip
7bit.gz: $(GEN_DIR)/$(OUT_7BIT).gz

$(GEN_DIR)/$(OUT_7BIT): $(SRC)
	$(7BIT) $(SRC) $(GEN_DIR)/$(OUT_7BIT)
$(GEN_DIR)/$(OUT_7BIT).zip: $(GEN_DIR)/$(OUT_7BIT)
	$(ZIP) -l $(GEN_DIR)/$(OUT_7BIT).zip $(GEN_DIR)/$(OUT_7BIT)
$(GEN_DIR)/$(OUT_7BIT).gz: $(GEN_DIR)/$(OUT_7BIT)
	$(GZIP) -c $(GEN_DIR)/$(OUT_7BIT) > $(GEN_DIR)/$(OUT_7BIT).gz


# deutsche HTML-Version
html: $(GEN_DIR)/$(OUT_HTML)
html.zip: $(GEN_DIR)/$(OUT_HTML).zip
html.gz: $(GEN_DIR)/$(OUT_HTML).gz

$(GEN_DIR)/$(OUT_HTML): $(SRC)
	$(HTML) $(SRC) $(GEN_DIR)/$(OUT_HTML)
$(GEN_DIR)/$(OUT_HTML).zip: $(GEN_DIR)/$(OUT_HTML)
	$(ZIP) -l $(GEN_DIR)/$(OUT_HTML).zip $(GEN_DIR)/$(OUT_HTML)
$(GEN_DIR)/$(OUT_HTML).gz: $(GEN_DIR)/$(OUT_HTML)
	$(GZIP) -c $(GEN_DIR)/$(OUT_HTML) > $(GEN_DIR)/$(OUT_HTML).gz


# deutsches Diff erzeugen
dif:
	[ ! -e $(SRC_DIFF) ] # Diff nur erzeugen, wenn noch nicht vorhanden
	diff -u $(OLD_SRC) $(SRC) > $(SRC_DIFF); echo -n

# deutsches Diff einspielen
pat:
	[ -e $(SRC_DIFF) ] # Nur patchen wenn diff vorhanden
	patch $(SRC) < $(SRC_DIFF)
	if [ -e $(SRC).orig ]; then mv $(SRC).orig $(OLD_SRC); fi

# Alle Fragen der deutschen FAQ als NEU deklarieren
new:
	$(NEW) $(SRC)

# Alle Fragen der deutschen FAQ als ALT deklarieren
old:
	$(OLD) $(SRC)


# englische 7Bit-Version
e7bit: $(GEN_DIR)/$(OUT_ENG_7BIT)
e7bit.zip: $(GEN_DIR)/$(OUT_ENG_7BIT).zip
e7bit.gz: $(GEN_DIR)/$(OUT_ENG_7BIT).gz

$(GEN_DIR)/$(OUT_ENG_7BIT): $(SRC)
	$(7BIT) $(SRC_ENG) $(GEN_DIR)/$(OUT_ENG_7BIT)
$(GEN_DIR)/$(OUT_ENG_7BIT).zip: $(GEN_DIR)/$(OUT_ENG_7BIT)
	$(ZIP) -l $(GEN_DIR)/$(OUT_ENG_7BIT).zip $(GEN_DIR)/$(OUT_ENG_7BIT)
$(GEN_DIR)/$(OUT_ENG_7BIT).gz: $(GEN_DIR)/$(OUT_ENG_7BIT)
	$(GZIP) -c $(GEN_DIR)/$(OUT_ENG_7BIT) > $(GEN_DIR)/$(OUT_ENG_7BIT).gz


# englische HTML-Version
ehtml: $(GEN_DIR)/$(OUT_ENG_HTML)
ehtml.zip: $(GEN_DIR)/$(OUT_ENG_HTML).zip
ehtml.gz: $(GEN_DIR)/$(OUT_ENG_HTML).gz

$(GEN_DIR)/$(OUT_ENG_HTML): $(SRC)
	$(HTML) $(SRC_ENG) $(GEN_DIR)/$(OUT_ENG_HTML)
$(GEN_DIR)/$(OUT_ENG_HTML).zip: $(GEN_DIR)/$(OUT_ENG_HTML)
	$(ZIP) -l $(GEN_DIR)/$(OUT_ENG_HTML).zip $(GEN_DIR)/$(OUT_ENG_HTML)
$(GEN_DIR)/$(OUT_ENG_HTML).gz: $(GEN_DIR)/$(OUT_ENG_HTML)
	$(GZIP) -c $(GEN_DIR)/$(OUT_ENG_HTML) > $(GEN_DIR)/$(OUT_ENG_HTML).gz


# englisches Diff erzeugen
edif:
	[ ! -e $(SRC_ENG_DIFF) ] # Diff nur erzeugen, wenn noch nicht vorhanden
	diff -u $(OLD_SRC_ENG) $(SRC_ENG) > $(SRC_ENG_DIFF); echo -n

# englisches Diff einspielen
epat:
	[ -e $(SRC_ENG_DIFF) ] # Nur patchen wenn diff vorhanden
	patch $(SRC_ENG) < $(SRC_ENG_DIFF)
	if [ -e $(SRC_ENG).orig ]; then mv $(SRC_ENG).orig $(OLD_SRC_ENG); fi

# Alle Fragen der englischen FAQ als NEU deklarieren
enew:
	$(NEW) $(SRC_ENG)

# Alle Fragen der englischen FAQ als ALT deklarieren
eold:
	$(OLD) $(SRC_ENG)


# Erzeugte Dateien installieren
install:
	if [ $(TARGET_DIR) != $(GEN_DIR) ]; then cp $(GEN_DIR)/* $(TARGET_DIR); fi


# Aufraeumen
clean:
	if [ -e *~ ]; then rm *~; fi
	if [ -e \#* ]; then rm \#*; fi
	cd $(GEN_DIR); if [ -e $(OUT_7BIT) ]; then rm $(OUT_7BIT); fi
	cd $(GEN_DIR); if [ -e $(OUT_HTML) ]; then rm $(OUT_HTML); fi
	cd $(GEN_DIR); if [ -e $(OUT_ENG_7BIT) ]; then rm $(OUT_ENG_7BIT); fi
	cd $(GEN_DIR); if [ -e $(OUT_ENG_HTML) ]; then rm $(OUT_ENG_HTML); fi
	cd $(GEN_DIR); if [ -e $(OUT_7BIT).zip ]; then rm $(OUT_7BIT).zip; fi
	cd $(GEN_DIR); if [ -e $(OUT_HTML).zip ]; then rm $(OUT_HTML).zip; fi
	cd $(GEN_DIR); if [ -e $(OUT_ENG_7BIT).zip ]; then rm $(OUT_ENG_7BIT).zip; fi
	cd $(GEN_DIR); if [ -e $(OUT_ENG_HTML).zip ]; then rm $(OUT_ENG_HTML).zip; fi
	cd $(GEN_DIR); if [ -e $(OUT_7BIT).gz ]; then rm $(OUT_7BIT).gz; fi
	cd $(GEN_DIR); if [ -e $(OUT_HTML).gz ]; then rm $(OUT_HTML).gz; fi
	cd $(GEN_DIR); if [ -e $(OUT_ENG_7BIT).gz ]; then rm $(OUT_ENG_7BIT).gz; fi
	cd $(GEN_DIR); if [ -e $(OUT_ENG_HTML).gz ]; then rm $(OUT_ENG_HTML).gz; fi
