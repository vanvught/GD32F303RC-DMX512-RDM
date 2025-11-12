ifneq ($(EXTRA_C_SOURCE_FILES),)
	EXTRA_C_OBJECTS := $(patsubst %.c,$(BUILD)%.o,$(EXTRA_C_SOURCE_FILES))
	# Directories (deduped), include both source-file dirs and EXTRA_SRCDIR
	EXTRA_C_DIRECTORIES := $(sort $(dir $(EXTRA_C_SOURCE_FILES)) $(EXTRA_SRCDIR))
	EXTRA_C_BUILD_DIRS := $(addprefix $(BUILD),$(EXTRA_C_DIRECTORIES))
endif

ifneq ($(EXTRA_C_SOURCE_FILES),)
	EXTRA_CPP_OBJECTS := $(patsubst %.cpp,$(BUILD)%.o,$(EXTRA_CPP_SOURCE_FILES))
	# Directories (deduped), include both source-file dirs and EXTRA_SRCDIR
	EXTRA_CPP_DIRECTORIES := $(sort $(dir $(EXTRA_CPP_SOURCE_FILES)) $(EXTRA_SRCDIR))
	EXTRA_CPP_BUILD_DIRS := $(addprefix $(BUILD),$(EXTRA_CPP_DIRECTORIES))
endif 