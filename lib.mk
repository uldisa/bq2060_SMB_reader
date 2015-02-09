###### core library
ARDUINO_CORE_PATH=$(ARD_HOME)/hardware/arduino/avr/cores/arduino
# Generate CPPFLAGS
ifeq ($(filter %-pc-cygwin,$(MAKE_HOST)),)
CORE_INCLUDES:=-I$(ARDUINO_CORE_PATH) -I$(ARDUINO_CORE_PATH)/../../variants/$(VARIANT)
else
CORE_INCLUDES:=-I$(shell cygpath -m $(ARDUINO_CORE_PATH)) -I$(shell cygpath -m $(ARDUINO_CORE_PATH)/../../variants/$(VARIANT))
endif
CPPFLAGS+=$(CORE_INCLUDES)

# Find core sources
LIBcore_C     = $(wildcard $(ARDUINO_CORE_PATH)/*.c)
LIBcore_CPP   = $(wildcard $(ARDUINO_CORE_PATH)/*.cpp)

###### external libraries
ARD_LIB_PATH=$(ARD_HOME)/libraries

# Find library sources
ifeq "$(filter clean,$(MAKECMDGOALS))" ""
$(foreach l,$(filter-out core,$(LIBS)),$(if $(wildcard $(ARD_LIB_PATH)/$l $(USER_LIB_PATH)/$l),,$(error Library $l no found in $(ARD_LIB_PATH)/$l)))
endif

define get_lib_sources
LIB$1_C     += $(foreach d,. utility src src/utility arch/avr arch/avr/utility,$(wildcard $2/$(d)/*.c))
LIB$1_CPP   += $(foreach d,. utility src src/utility arch/avr arch/avr/utility,$(wildcard $2/$(d)/*.cpp))
endef
$(foreach l,$(filter-out core,$(LIBS)),$(eval $(call get_lib_sources,$(l),$(ARD_LIB_PATH)/$(l))))
$(foreach l,$(filter-out core,$(LIBS)),$(eval $(call get_lib_sources,$(l),$(USER_LIB_PATH)/$(l))))

# Generate CPPFLAGS
LIB_INCLUDES:=$(foreach l,$(filter-out core,$(LIBS)),$(foreach d,. src arch/avr,$(dir $(wildcard $(ARD_LIB_PATH)/$(l)/$(d)/*.h $(USER_LIB_PATH)/$(l)/$(d)/*.h))))
ifeq ($(filter %-pc-cygwin,$(MAKE_HOST)),)
LIB_INCLUDES:=$(addprefix -I,$(LIB_INCLUDES))
else
LIB_INCLUDES:=$(addprefix -I,$(foreach i,$(LIB_INCLUDES),$(shell cygpath -m "$i")))
endif
CPPFLAGS+=$(LIB_INCLUDES)

# Generate .o:.c rules
define c_compile_rules
LIB$1_OBJS       += $$(OBJDIR)/$1/$(notdir $(2:.c=.o))
$$(OBJDIR)/$1/$(notdir $(2:.c=.o)): $2 |$$(OBJDIR)/$1/
	$(value CC_CMD)

endef
$(foreach l,$(LIBS),$(foreach c,$(LIB$(l)_C),$(eval $(call c_compile_rules,$(l),$(c)))))

# Generate .o:.cpp rules
define cpp_compile_rules
LIB$1_OBJS       += $$(OBJDIR)/$1/$(notdir $(2:.cpp=.o))
$$(OBJDIR)/$1/$(notdir $(2:.cpp=.o)): $2 |$$(OBJDIR)/$1/
	$(value CXX_CMD)

endef
$(foreach l,$(LIBS),$(foreach c,$(LIB$(l)_CPP),$(eval $(call cpp_compile_rules,$(l),$(c)))))

# Generate rules for order only directory prerequisites
$(addprefix $(OBJDIR)/,$(addsuffix /,$(LIBS))):
	mkdir -p $@

define create_lib
lib$1.a:$$(LIB$1_OBJS)
	$$(AR) rcs $$@ $$^
endef
$(foreach l,$(LIBS),$(eval $(call create_lib,$(l))))

clean::
	rm -f $(foreach l,$(LIBS),lib$(l).a)
	rm -Rf $(foreach l,$(LIBS),$(l)/)

