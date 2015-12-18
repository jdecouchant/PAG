#ifndef __parameters_h__
#define __parameters_h__

class Parameters {
private:
  char paramFileName[200];
  int selectedCategory;
  
  struct definitionDesc {
    char name[50];
    char value[200];
    bool accessed;
    struct definitionDesc *next;
  };
  
  struct categoryDesc {
    char name[200];
    int numDefinitions;
    struct definitionDesc *first;
  };

  struct categoryDesc *categories;    
  int categoryListSize;
  int numCategories;
  
public:
  Parameters(const char *filename);
  ~Parameters();
  bool selectSection(const char *newCategory);
  bool existsSetting(const char *name);
  int getAsInt(const char *name);
  void getAsString(const char *name, char *buf, int bufsize);
  double getAsDouble(const char *name);
  bool getAsBoolean(const char *name);
  int numSections();
  int numSettingsInSection();
  const char *getSectionByIndex(int index);
  void getSettingByIndex(int index, char **name, char **value);
  void assertAllAccessed();
  void write(const char *filename);
};

#endif /* defined(__parameters_h__) */
