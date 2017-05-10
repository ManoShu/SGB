$(info  Starting building process)

# include configurations

include Makefile.conf

$(info - Makefile.conf loaded)

# find project files

H_FILES   :=    $(shell find -L $(INC_DIRECTORY) -maxdepth $(INC_DIRECTORY_DEPTH) -name '*.h' -exec dirname {} \; | sed 's/ /\\ /g' | uniq)

C_FILES   :=    $(shell find $(SRC_DIRECTORY) -maxdepth $(SRC_DIRECTORY_DEPTH) -name '*.c' -type f | sed 's/ /\\ /g' | uniq)

CXX_FILES :=    $(shell find $(SRC_DIRECTORY) -maxdepth $(SRC_DIRECTORY_DEPTH) -name '*.cpp' -type f | sed 's/ /\\ /g' | uniq)

O_FILES   :=    $(C_FILES:.c=.o)

O_FILES   +=    $(CXX_FILES:.cpp=.o)

H_FILES   :=    $(notdir  $(H_FILES))

C_FILES   :=    $(notdir  $(C_FILES))

CXX_FILES :=    $(notdir  $(CXX_FILES))

INCLUDES  :=    $(H_FILES:%=-I%)

LIBRARY_NAME := lib$(PROJECT_NAME).so

FULL_LIBRARY_NAME := $(LIBRARY_NAME).$(PROJECT_VERSION)

$(info  - Project Files Loaded)


ifeq ($(DEBUG),yes)

   $(info - Debug flag added [makefile.conf DEBUG = yes])

   CFLAGS = -g $(CFLAGS)

endif


ifeq ($(IS_LIBRARY),yes)

   $(info - Set Parameters for Shared Library build process)

   ALL_PARAMETERS = $(FULL_LIBRARY_NAME) clean

   ALL_TYPE = $(FULL_LIBRARY_NAME): $(O_FILES)

   LIBFLAGS = -shared -Wl,-soname,lib$(PROJECT_NAME).so

   CFLAGS :=  -fPIC $(CFLAGS)

   CXXFLAGS := -fPIC $(CXXFLAGS)
else

   $(info - Set Parameters for Application build process)

   ALL_PARAMETERS = $(PROJECT_NAME) clean

   ALL_TYPE = $(PROJECT_NAME): $(O_FILES)

   LIBFLAGS =

endif

# Build Process

all: $(ALL_PARAMETERS)

$(ALL_TYPE)
	@echo -  [OUTPUT][CXX] $@ @[$(BIN_DIRECTORY)]
	@mkdir -p $(BIN_DIRECTORY)
	@$(CXX) $(CFLAGS) $(INCLUDES) $(LDFLAGS) $(LIBFLAGS) -o $(BIN_DIRECTORY)/$@ $^ $(LDLIBS)

%.o: %.c
	@echo -  [CC] $@
	@$(CC) $(CFLAGS) -c $(INCLUDES) -o $@ $< $(LFLAGS)

%.o: %.cpp
	@echo -  [CXX] $@
	@$(CXX) $(CXXFLAGS) -c $(INCLUDES) -o $@ $< $(LFLAGS)

# Clear Objects

clean:
	$(info - Remove all .o [object] files)
	@find . -name \*.o -type f -delete

# Clear Objects & Executables

cleanall: clean cleanexample cleandocs
	$(info - Remove $(BIN_DIRECTORY) directory)
	@rm -f -R  $(BIN_DIRECTORY)
	$(info - Done.)

install: 
	$(info - Copying header file(s))
	@cp $(INC_DIRECTORY)/*.h $(INS_HEARERS_DIRECTORY)
	$(info - Copying library file(s))
	@cp $(BIN_DIRECTORY)/$(FULL_LIBRARY_NAME) $(INS_SO_DIRECTORY)$(FULL_LIBRARY_NAME)
	$(info - Linking library file(s))
	@rm -f $(INS_SO_DIRECTORY)$(LIBRARY_NAME)
	@ln -s $(INS_SO_DIRECTORY)$(FULL_LIBRARY_NAME) $(INS_SO_DIRECTORY)$(LIBRARY_NAME)
	$(info - Executing ldconfig...)
	@ldconfig
	$(info - Done.)

uninstall:
	$(info - Deleting header directory)
	@rm -f -R $(INS_HEARERS_DIRECTORY)$(PROJECT_NAME)*
	$(info - Deleting library directory)
	@rm -f -R  $(INS_SO_DIRECTORY)$(LIBRARY_NAME)*

docs:
	$(info - Generating doc files...)
	@cd $(DOC_DIRECTORY)/; doxygen Doxyfile;
	$(info - Done.)

cleandocs:
	$(info - Removing doc files...)
#TODO:maybe would be better to delete everything except the doxyfile itself
	@cd $(DOC_DIRECTORY)/; rm -f -R html/; rm -f doxygen_sqlite3.db;
	$(info - Done.)

buildexample:
	$(info - Compiling example application...)
	@cd $(EXAMPLE_DIRECTORY)/; $(CXX) -o example *.cpp -std=c++11 -lSGB -lSDL2;
	$(info - Done.)
	
cleanexample:
	$(info - Removing example build...)
	@cd $(EXAMPLE_DIRECTORY)/; rm -f example;
	$(info - Done.)

runexample: buildexample
	$(info - Running example...)
	@cd $(EXAMPLE_DIRECTORY)/; ./example;
	$(info - Done.)
