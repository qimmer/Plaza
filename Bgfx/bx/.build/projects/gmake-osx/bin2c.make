# GNU Make project makefile autogenerated by GENie
ifndef config
  config=debug32
endif

ifndef verbose
  SILENT = @
endif

SHELLTYPE := msdos
ifeq (,$(ComSpec)$(COMSPEC))
  SHELLTYPE := posix
endif
ifeq (/bin,$(findstring /bin,$(SHELL)))
  SHELLTYPE := posix
endif
ifeq (/bin,$(findstring /bin,$(MAKESHELL)))
  SHELLTYPE := posix
endif

ifeq (posix,$(SHELLTYPE))
  MKDIR = $(SILENT) mkdir -p "$(1)"
  COPY  = $(SILENT) cp -fR "$(1)" "$(2)"
  RM    = $(SILENT) rm -f "$(1)"
else
  MKDIR = $(SILENT) mkdir "$(subst /,\\,$(1))" 2> nul || exit 0
  COPY  = $(SILENT) copy /Y "$(subst /,\\,$(1))" "$(subst /,\\,$(2))"
  RM    = $(SILENT) del /F "$(subst /,\\,$(1))" 2> nul || exit 0
endif

CC  = gcc
CXX = g++
AR  = ar

ifndef RESCOMP
  ifdef WINDRES
    RESCOMP = $(WINDRES)
  else
    RESCOMP = windres
  endif
endif

MAKEFILE = bin2c.make

ifeq ($(config),debug32)
  OBJDIR              = ../../osx32_clang/obj/x32/Debug/bin2c
  TARGETDIR           = ../../osx32_clang/bin
  TARGET              = $(TARGETDIR)/bin2cDebug
  DEFINES            += -D__STDC_LIMIT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_CONSTANT_MACROS -D_DEBUG
  INCLUDES           += -I"../../../include/compat/osx" -I"../../../include"
  INCLUDES           +=
  ALL_CPPFLAGS       += $(CPPFLAGS) -MMD -MP -MP $(DEFINES) $(INCLUDES)
  ALL_ASMFLAGS       += $(ASMFLAGS) $(CFLAGS) $(ALL_CPPFLAGS) $(ARCH) -Wall -Wextra -fomit-frame-pointer -g -m32 -Wshadow -m32 -Wfatal-errors -msse2 -Wunused-value -Wundef
  ALL_CFLAGS         += $(CFLAGS) $(ALL_CPPFLAGS) $(ARCH) -Wall -Wextra -fomit-frame-pointer -g -m32 -Wshadow -m32 -Wfatal-errors -msse2 -Wunused-value -Wundef
  ALL_CXXFLAGS       += $(CXXFLAGS) $(CFLAGS) $(ALL_CPPFLAGS) $(ARCH) -Wall -Wextra -fomit-frame-pointer -g -m32 -fno-rtti -fno-exceptions -Wshadow -m32 -Wfatal-errors -msse2 -Wunused-value -Wundef -std=c++11
  ALL_OBJCFLAGS      += $(CFLAGS) $(ALL_CPPFLAGS) $(ARCH) -Wall -Wextra -fomit-frame-pointer -g -m32 -Wshadow -m32 -Wfatal-errors -msse2 -Wunused-value -Wundef
  ALL_OBJCPPFLAGS    += $(CXXFLAGS) $(CFLAGS) $(ALL_CPPFLAGS) $(ARCH) -Wall -Wextra -fomit-frame-pointer -g -m32 -fno-rtti -fno-exceptions -Wshadow -m32 -Wfatal-errors -msse2 -Wunused-value -Wundef -std=c++11
  ALL_RESFLAGS       += $(RESFLAGS) $(DEFINES) $(INCLUDES)
  ALL_LDFLAGS        += $(LDFLAGS) -L../../osx32_clang/bin -m32
  LDDEPS             += ../../osx32_clang/bin/libbxDebug.a
  LIBS               += $(LDDEPS)
  EXTERNAL_LIBS      +=
  LINKCMD             = $(CXX) -o $(TARGET) $(OBJECTS) $(RESOURCES) $(ARCH) $(ALL_LDFLAGS) $(LIBS)
  OBJECTS := \
	$(OBJDIR)/tools/bin2c/bin2c.o \

  define PREBUILDCMDS
  endef
  define PRELINKCMDS
  endef
  define POSTBUILDCMDS
  endef
endif

ifeq ($(config),release32)
  OBJDIR              = ../../osx32_clang/obj/x32/Release/bin2c
  TARGETDIR           = ../../osx32_clang/bin
  TARGET              = $(TARGETDIR)/bin2cRelease
  DEFINES            += -D__STDC_LIMIT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_CONSTANT_MACROS -DNDEBUG
  INCLUDES           += -I"../../../include/compat/osx" -I"../../../include"
  INCLUDES           +=
  ALL_CPPFLAGS       += $(CPPFLAGS) -MMD -MP -MP $(DEFINES) $(INCLUDES)
  ALL_ASMFLAGS       += $(ASMFLAGS) $(CFLAGS) $(ALL_CPPFLAGS) $(ARCH) -Wall -Wextra -fomit-frame-pointer -g -O3 -m32 -Wshadow -m32 -Wfatal-errors -msse2 -Wunused-value -Wundef
  ALL_CFLAGS         += $(CFLAGS) $(ALL_CPPFLAGS) $(ARCH) -Wall -Wextra -fomit-frame-pointer -g -O3 -m32 -Wshadow -m32 -Wfatal-errors -msse2 -Wunused-value -Wundef
  ALL_CXXFLAGS       += $(CXXFLAGS) $(CFLAGS) $(ALL_CPPFLAGS) $(ARCH) -Wall -Wextra -fomit-frame-pointer -g -O3 -m32 -fno-rtti -fno-exceptions -Wshadow -m32 -Wfatal-errors -msse2 -Wunused-value -Wundef -std=c++11
  ALL_OBJCFLAGS      += $(CFLAGS) $(ALL_CPPFLAGS) $(ARCH) -Wall -Wextra -fomit-frame-pointer -g -O3 -m32 -Wshadow -m32 -Wfatal-errors -msse2 -Wunused-value -Wundef
  ALL_OBJCPPFLAGS    += $(CXXFLAGS) $(CFLAGS) $(ALL_CPPFLAGS) $(ARCH) -Wall -Wextra -fomit-frame-pointer -g -O3 -m32 -fno-rtti -fno-exceptions -Wshadow -m32 -Wfatal-errors -msse2 -Wunused-value -Wundef -std=c++11
  ALL_RESFLAGS       += $(RESFLAGS) $(DEFINES) $(INCLUDES)
  ALL_LDFLAGS        += $(LDFLAGS) -L../../osx32_clang/bin -m32
  LDDEPS             += ../../osx32_clang/bin/libbxRelease.a
  LIBS               += $(LDDEPS)
  EXTERNAL_LIBS      +=
  LINKCMD             = $(CXX) -o $(TARGET) $(OBJECTS) $(RESOURCES) $(ARCH) $(ALL_LDFLAGS) $(LIBS)
  OBJECTS := \
	$(OBJDIR)/tools/bin2c/bin2c.o \

  define PREBUILDCMDS
  endef
  define PRELINKCMDS
  endef
  define POSTBUILDCMDS
  endef
endif

ifeq ($(config),debug64)
  OBJDIR              = ../../osx64_clang/obj/x64/Debug/bin2c
  TARGETDIR           = ../../osx64_clang/bin
  TARGET              = $(TARGETDIR)/bin2cDebug
  DEFINES            += -D__STDC_LIMIT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_CONSTANT_MACROS -D_DEBUG
  INCLUDES           += -I"../../../include/compat/osx" -I"../../../include"
  INCLUDES           +=
  ALL_CPPFLAGS       += $(CPPFLAGS) -MMD -MP -MP $(DEFINES) $(INCLUDES)
  ALL_ASMFLAGS       += $(ASMFLAGS) $(CFLAGS) $(ALL_CPPFLAGS) $(ARCH) -Wall -Wextra -fomit-frame-pointer -g -m64 -Wshadow -m64 -Wfatal-errors -msse2 -Wunused-value -Wundef
  ALL_CFLAGS         += $(CFLAGS) $(ALL_CPPFLAGS) $(ARCH) -Wall -Wextra -fomit-frame-pointer -g -m64 -Wshadow -m64 -Wfatal-errors -msse2 -Wunused-value -Wundef
  ALL_CXXFLAGS       += $(CXXFLAGS) $(CFLAGS) $(ALL_CPPFLAGS) $(ARCH) -Wall -Wextra -fomit-frame-pointer -g -m64 -fno-rtti -fno-exceptions -Wshadow -m64 -Wfatal-errors -msse2 -Wunused-value -Wundef -std=c++11
  ALL_OBJCFLAGS      += $(CFLAGS) $(ALL_CPPFLAGS) $(ARCH) -Wall -Wextra -fomit-frame-pointer -g -m64 -Wshadow -m64 -Wfatal-errors -msse2 -Wunused-value -Wundef
  ALL_OBJCPPFLAGS    += $(CXXFLAGS) $(CFLAGS) $(ALL_CPPFLAGS) $(ARCH) -Wall -Wextra -fomit-frame-pointer -g -m64 -fno-rtti -fno-exceptions -Wshadow -m64 -Wfatal-errors -msse2 -Wunused-value -Wundef -std=c++11
  ALL_RESFLAGS       += $(RESFLAGS) $(DEFINES) $(INCLUDES)
  ALL_LDFLAGS        += $(LDFLAGS) -L../../osx64_clang/bin -m64
  LDDEPS             += ../../osx64_clang/bin/libbxDebug.a
  LIBS               += $(LDDEPS)
  EXTERNAL_LIBS      +=
  LINKCMD             = $(CXX) -o $(TARGET) $(OBJECTS) $(RESOURCES) $(ARCH) $(ALL_LDFLAGS) $(LIBS)
  OBJECTS := \
	$(OBJDIR)/tools/bin2c/bin2c.o \

  define PREBUILDCMDS
  endef
  define PRELINKCMDS
  endef
  define POSTBUILDCMDS
  endef
endif

ifeq ($(config),release64)
  OBJDIR              = ../../osx64_clang/obj/x64/Release/bin2c
  TARGETDIR           = ../../osx64_clang/bin
  TARGET              = $(TARGETDIR)/bin2cRelease
  DEFINES            += -D__STDC_LIMIT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_CONSTANT_MACROS -DNDEBUG
  INCLUDES           += -I"../../../include/compat/osx" -I"../../../include"
  INCLUDES           +=
  ALL_CPPFLAGS       += $(CPPFLAGS) -MMD -MP -MP $(DEFINES) $(INCLUDES)
  ALL_ASMFLAGS       += $(ASMFLAGS) $(CFLAGS) $(ALL_CPPFLAGS) $(ARCH) -Wall -Wextra -fomit-frame-pointer -g -O3 -m64 -Wshadow -m64 -Wfatal-errors -msse2 -Wunused-value -Wundef
  ALL_CFLAGS         += $(CFLAGS) $(ALL_CPPFLAGS) $(ARCH) -Wall -Wextra -fomit-frame-pointer -g -O3 -m64 -Wshadow -m64 -Wfatal-errors -msse2 -Wunused-value -Wundef
  ALL_CXXFLAGS       += $(CXXFLAGS) $(CFLAGS) $(ALL_CPPFLAGS) $(ARCH) -Wall -Wextra -fomit-frame-pointer -g -O3 -m64 -fno-rtti -fno-exceptions -Wshadow -m64 -Wfatal-errors -msse2 -Wunused-value -Wundef -std=c++11
  ALL_OBJCFLAGS      += $(CFLAGS) $(ALL_CPPFLAGS) $(ARCH) -Wall -Wextra -fomit-frame-pointer -g -O3 -m64 -Wshadow -m64 -Wfatal-errors -msse2 -Wunused-value -Wundef
  ALL_OBJCPPFLAGS    += $(CXXFLAGS) $(CFLAGS) $(ALL_CPPFLAGS) $(ARCH) -Wall -Wextra -fomit-frame-pointer -g -O3 -m64 -fno-rtti -fno-exceptions -Wshadow -m64 -Wfatal-errors -msse2 -Wunused-value -Wundef -std=c++11
  ALL_RESFLAGS       += $(RESFLAGS) $(DEFINES) $(INCLUDES)
  ALL_LDFLAGS        += $(LDFLAGS) -L../../osx64_clang/bin -m64
  LDDEPS             += ../../osx64_clang/bin/libbxRelease.a
  LIBS               += $(LDDEPS)
  EXTERNAL_LIBS      +=
  LINKCMD             = $(CXX) -o $(TARGET) $(OBJECTS) $(RESOURCES) $(ARCH) $(ALL_LDFLAGS) $(LIBS)
  OBJECTS := \
	$(OBJDIR)/tools/bin2c/bin2c.o \

  define PREBUILDCMDS
  endef
  define PRELINKCMDS
  endef
  define POSTBUILDCMDS
  endef
endif

ifeq ($(config),debug)
  OBJDIR              = obj/Debug/bin2c
  TARGETDIR           = ../../../scripts
  TARGET              = $(TARGETDIR)/bin2cDebug
  DEFINES            += -D__STDC_LIMIT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_CONSTANT_MACROS -D_DEBUG
  INCLUDES           += -I"../../../include/compat/osx" -I"../../../include"
  INCLUDES           +=
  ALL_CPPFLAGS       += $(CPPFLAGS) -MMD -MP -MP $(DEFINES) $(INCLUDES)
  ALL_ASMFLAGS       += $(ASMFLAGS) $(CFLAGS) $(ALL_CPPFLAGS) $(ARCH) -Wall -Wextra -fomit-frame-pointer -g -Wshadow -Wfatal-errors -msse2 -Wunused-value -Wundef
  ALL_CFLAGS         += $(CFLAGS) $(ALL_CPPFLAGS) $(ARCH) -Wall -Wextra -fomit-frame-pointer -g -Wshadow -Wfatal-errors -msse2 -Wunused-value -Wundef
  ALL_CXXFLAGS       += $(CXXFLAGS) $(CFLAGS) $(ALL_CPPFLAGS) $(ARCH) -Wall -Wextra -fomit-frame-pointer -g -fno-rtti -fno-exceptions -Wshadow -Wfatal-errors -msse2 -Wunused-value -Wundef -std=c++11
  ALL_OBJCFLAGS      += $(CFLAGS) $(ALL_CPPFLAGS) $(ARCH) -Wall -Wextra -fomit-frame-pointer -g -Wshadow -Wfatal-errors -msse2 -Wunused-value -Wundef
  ALL_OBJCPPFLAGS    += $(CXXFLAGS) $(CFLAGS) $(ALL_CPPFLAGS) $(ARCH) -Wall -Wextra -fomit-frame-pointer -g -fno-rtti -fno-exceptions -Wshadow -Wfatal-errors -msse2 -Wunused-value -Wundef -std=c++11
  ALL_RESFLAGS       += $(RESFLAGS) $(DEFINES) $(INCLUDES)
  ALL_LDFLAGS        += $(LDFLAGS) -L../../../scripts
  LDDEPS             += ../../../scripts/libbxDebug.a
  LIBS               += $(LDDEPS)
  EXTERNAL_LIBS      +=
  LINKCMD             = $(CXX) -o $(TARGET) $(OBJECTS) $(RESOURCES) $(ARCH) $(ALL_LDFLAGS) $(LIBS)
  OBJECTS := \
	$(OBJDIR)/tools/bin2c/bin2c.o \

  define PREBUILDCMDS
  endef
  define PRELINKCMDS
  endef
  define POSTBUILDCMDS
  endef
endif

ifeq ($(config),release)
  OBJDIR              = obj/Release/bin2c
  TARGETDIR           = ../../../scripts
  TARGET              = $(TARGETDIR)/bin2cRelease
  DEFINES            += -D__STDC_LIMIT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_CONSTANT_MACROS -DNDEBUG
  INCLUDES           += -I"../../../include/compat/osx" -I"../../../include"
  INCLUDES           +=
  ALL_CPPFLAGS       += $(CPPFLAGS) -MMD -MP -MP $(DEFINES) $(INCLUDES)
  ALL_ASMFLAGS       += $(ASMFLAGS) $(CFLAGS) $(ALL_CPPFLAGS) $(ARCH) -Wall -Wextra -fomit-frame-pointer -g -O3 -Wshadow -Wfatal-errors -msse2 -Wunused-value -Wundef
  ALL_CFLAGS         += $(CFLAGS) $(ALL_CPPFLAGS) $(ARCH) -Wall -Wextra -fomit-frame-pointer -g -O3 -Wshadow -Wfatal-errors -msse2 -Wunused-value -Wundef
  ALL_CXXFLAGS       += $(CXXFLAGS) $(CFLAGS) $(ALL_CPPFLAGS) $(ARCH) -Wall -Wextra -fomit-frame-pointer -g -O3 -fno-rtti -fno-exceptions -Wshadow -Wfatal-errors -msse2 -Wunused-value -Wundef -std=c++11
  ALL_OBJCFLAGS      += $(CFLAGS) $(ALL_CPPFLAGS) $(ARCH) -Wall -Wextra -fomit-frame-pointer -g -O3 -Wshadow -Wfatal-errors -msse2 -Wunused-value -Wundef
  ALL_OBJCPPFLAGS    += $(CXXFLAGS) $(CFLAGS) $(ALL_CPPFLAGS) $(ARCH) -Wall -Wextra -fomit-frame-pointer -g -O3 -fno-rtti -fno-exceptions -Wshadow -Wfatal-errors -msse2 -Wunused-value -Wundef -std=c++11
  ALL_RESFLAGS       += $(RESFLAGS) $(DEFINES) $(INCLUDES)
  ALL_LDFLAGS        += $(LDFLAGS) -L../../../scripts
  LDDEPS             += ../../../scripts/libbxRelease.a
  LIBS               += $(LDDEPS)
  EXTERNAL_LIBS      +=
  LINKCMD             = $(CXX) -o $(TARGET) $(OBJECTS) $(RESOURCES) $(ARCH) $(ALL_LDFLAGS) $(LIBS)
  OBJECTS := \
	$(OBJDIR)/tools/bin2c/bin2c.o \

  define PREBUILDCMDS
  endef
  define PRELINKCMDS
  endef
  define POSTBUILDCMDS
  endef
endif

OBJDIRS := \
	$(OBJDIR) \
	$(OBJDIR)/tools/bin2c \

RESOURCES := \

.PHONY: clean prebuild prelink

all: $(OBJDIRS) $(TARGETDIR) prebuild prelink $(TARGET)
	@:

$(TARGET): $(GCH) $(OBJECTS) $(LDDEPS) $(EXTERNAL_LIBS) $(RESOURCES) | $(TARGETDIR) $(OBJDIRS)
	@echo Linking bin2c
	$(SILENT) $(LINKCMD)
	$(POSTBUILDCMDS)

$(TARGETDIR):
	@echo Creating $(TARGETDIR)
	-$(call MKDIR,$(TARGETDIR))

$(OBJDIRS):
	@echo Creating $(@)
	-$(call MKDIR,$@)

clean:
	@echo Cleaning bin2c
ifeq (posix,$(SHELLTYPE))
	$(SILENT) rm -f  $(TARGET)
	$(SILENT) rm -rf $(OBJDIR)
else
	$(SILENT) if exist $(subst /,\\,$(TARGET)) del $(subst /,\\,$(TARGET))
	$(SILENT) if exist $(subst /,\\,$(OBJDIR)) rmdir /s /q $(subst /,\\,$(OBJDIR))
endif

prebuild:
	$(PREBUILDCMDS)

prelink:
	$(PRELINKCMDS)

ifneq (,$(PCH))
$(GCH): $(PCH) $(MAKEFILE) | $(OBJDIR)
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) -x c++-header $(DEFINES) $(INCLUDES) -o "$@" -c "$<"

$(GCH_OBJC): $(PCH) $(MAKEFILE) | $(OBJDIR)
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_OBJCPPFLAGS) -x objective-c++-header $(DEFINES) $(INCLUDES) -o "$@" -c "$<"
endif

$(OBJDIR)/tools/bin2c/bin2c.o: ../../../tools/bin2c/bin2c.cpp $(GCH) $(MAKEFILE)
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -c "$<"

-include $(OBJECTS:%.o=%.d)
ifneq (,$(PCH))
  -include $(OBJDIR)/$(notdir $(PCH)).d
  -include $(OBJDIR)/$(notdir $(PCH))_objc.d
endif
