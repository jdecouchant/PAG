#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "peerreview/tools/parameters.h"

#ifdef panic
#undef panic
#endif
#define panic(a...) do { fprintf(stderr, a); fprintf(stderr, "\n"); abort(); } while (0)

Parameters::Parameters(const char *filename)
{
  numCategories = 0;
  selectedCategory = -1;
  
  categoryListSize = 10;
  categories = (struct categoryDesc*) malloc(sizeof(struct categoryDesc) * categoryListSize);

  FILE *infile;
  if (filename) {
    strncpy(paramFileName, filename, sizeof(paramFileName));
    infile = fopen(filename, "r");
    if (!infile)
      panic("Cannot read parameter file: '%s'", filename);
  } else {
    strcpy(paramFileName, "stdin");
    infile = stdin;
  }
  
  int lineno = 0;
  char linebuf[500];
  while (fgets(linebuf, sizeof(linebuf), infile)) {
    lineno ++;
    char *p = &linebuf[0];
    while (*p) {
      if (strchr("\r\n#", *p))
        *p = 0;
      else 
        p++;
    }
    
    p = &linebuf[0];
    while (*p && isspace(*p))
      p++;
    if (!*p)
      continue;
    if (*p == '[') {
      while (numCategories >= categoryListSize) {
        struct categoryDesc *oldCategories = categories;
        int oldCategoryListSize = categoryListSize;
        categoryListSize *= 2;
        categories = (struct categoryDesc*) malloc(sizeof(struct categoryDesc) * categoryListSize);
        if (!categories)
          panic("%s:%d: Out of memory", filename, lineno);
        
        for (int i=0; i<oldCategoryListSize; i++)
          categories[i] = oldCategories[i];
        
        free(oldCategories);
      }
      
      char *thisCategoryName = &p[1];
      strtok(thisCategoryName, "]");
      for (int i=0; i<numCategories; i++)
        if (!strcmp(thisCategoryName, categories[i].name))
          panic("%s:%d: Duplicate category: '%s'", filename, lineno, thisCategoryName);
          
      strcpy(categories[numCategories].name, thisCategoryName);
      categories[numCategories].numDefinitions = 0;
      categories[numCategories].first = NULL;
      numCategories ++;
    } else {
      if (!strchr(p, '='))
        panic("%s:%d: Unknown setting: '%s'", filename, lineno, p);
      if (numCategories < 1)
        panic("%s:%d: Global settings not allowed", filename, lineno);
      
      char *equ = strchr(p, '=');
      *equ++ = 0;
      char *namepart = p;
      char *valuepart = equ;
      while (*namepart && isspace(*namepart))
        namepart ++;
      while (*valuepart && isspace(*valuepart))
        valuepart ++;
      while (*namepart && isspace(namepart[strlen(namepart)-1]))
        namepart[strlen(namepart)-1] = 0;
      while (*valuepart && isspace(valuepart[strlen(valuepart)-1]))
        valuepart[strlen(valuepart)-1] = 0;
      
      struct definitionDesc *newDef = (struct definitionDesc*) malloc(sizeof(struct definitionDesc));
      if (!newDef)
        panic("%s:%d: Out of memory", filename, lineno);
        
      if (categories[numCategories-1].first) {
        struct definitionDesc *iter = categories[numCategories-1].first;
        while (iter->next)
          iter = iter->next;
        iter->next = newDef;
      } else {
        categories[numCategories-1].first = newDef;
      }
        
      strncpy(newDef->name, namepart, sizeof(newDef->name));
      strncpy(newDef->value, valuepart, sizeof(newDef->value));
      newDef->accessed = false;
      newDef->next = NULL;
      categories[numCategories-1].numDefinitions ++;
    }
  }

  if (infile != stdin)  
    fclose(infile);
}

Parameters::~Parameters()
{
  for (int i=0; i<numCategories; i++) {
    struct definitionDesc *iter = categories[i].first;
    while (iter) {
      struct definitionDesc *next = iter->next;
      free(iter);
      iter = next;
    }
  }
  
  free(categories);
}

bool Parameters::selectSection(const char *newCategory)
{
  for (int i=0; i<numCategories; i++) {
    if (!strcmp(categories[i].name, newCategory)) {
      selectedCategory = i;
      return true;
    }
  }
  
  return false;
}

bool Parameters::existsSetting(const char *name)
{
  if (selectedCategory < 0)
    return false;
    
  struct definitionDesc *iter = categories[selectedCategory].first;
  while (iter) {
    if (!strcmp(iter->name, name))
      return true;
      
    iter = iter->next;
  }
  
  return false;
}

void Parameters::getAsString(const char *name, char *buf, int bufsize)
{
  if (selectedCategory < 0)
    panic("No category selected");

  struct definitionDesc *iter = categories[selectedCategory].first;
  while (iter) {
    if (!strcmp(iter->name, name)) {
      strncpy(buf, iter->value, bufsize);
      iter->accessed = true;
      return;
    }
    
    iter = iter->next;
  }
  
  panic("%s: Missing required setting '%s' in section [%s]", paramFileName, name, categories[selectedCategory].name);
}

int Parameters::getAsInt(const char *name)
{
  char buf[20];
  getAsString(name, buf, sizeof(buf));
  return atoi(buf);
}

double Parameters::getAsDouble(const char *name)
{
  char buf[20];
  getAsString(name, buf, sizeof(buf));
  return atof(buf);
}

bool Parameters::getAsBoolean(const char *name)
{
  char buf[20];
  getAsString(name, buf, sizeof(buf));
  return (!strcasecmp(buf, "yes") || !strcasecmp(buf, "on") || !strcasecmp(buf, "true"));
}

void Parameters::assertAllAccessed()
{
  for (int i=0; i<numCategories; i++) {
    struct definitionDesc *iter = categories[i].first;
    while (iter) {
      if (!iter->accessed)
        panic("%s: Unknown setting '%s' in section [%s]", paramFileName, iter->name, categories[i].name);
      
      iter = iter->next;
    }
  }
}

int Parameters::numSettingsInSection()
{
  assert(selectedCategory >= 0);
  return categories[selectedCategory].numDefinitions;
}

int Parameters::numSections()
{
  return numCategories;
}

const char *Parameters::getSectionByIndex(int index)
{
  assert((0<=index) && (index<=numCategories));
  return categories[index].name;
}

void Parameters::getSettingByIndex(int index, char **name, char **value)
{
  assert((selectedCategory >= 0) && (index >= 0));
  
  struct definitionDesc *iter = categories[selectedCategory].first;
  for (int i=0; i<index; i++) {
    if (!iter) 
      break;
    
    iter = iter->next;
  }
  
  if (iter) {
    *name = iter->name;
    *value = iter->value;
    iter->accessed = true;
  } else {
    *name = "";
    *value = "";
  } 
}

void Parameters::write(const char *filename)
{
  FILE *outfile;
  if (filename) {
    outfile = fopen(filename, "w+");
    if (!outfile)
      panic("Cannot write parameter file: '%s'", filename);
  } else {
    outfile = stdout;
  }

  for (int i=0; i<numCategories; i++) {
    fprintf(outfile, "[%s]\n", categories[i].name);

    struct definitionDesc *iter = categories[i].first;
    while (iter) {
      fprintf(outfile, "%s=%s\n", iter->name, iter->value);
      iter = iter->next;
    }
    
    if (i<(numCategories-1))
      fprintf(outfile, "\n");
  }

  if (outfile != stdout)
    fclose(outfile);
}
