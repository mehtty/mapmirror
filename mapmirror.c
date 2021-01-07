#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.c"

#define VERSION "0.1"
#define MAX_FIELDS 200

const char * TEX_TAG = "[textures]";
const char * FIELD_TAG = "[fields]";
const char COMMENT_CHAR = '#';

typedef struct EntField {
    char * fieldname;
    char * fieldvalue;
} ent_field;

typedef struct BrushFace {
    char * prefix;
    char * texture;
    char * suffix;
} brush_face;

typedef struct TextureReplace {
    char from[20];
    char to[20];
    struct TextureReplace * next;
} textures;

typedef struct FieldReplace {
    char entname[100];
    char fieldname[100];
    char from[200];
    char to[200];
    struct FieldReplace * next;
} fields;

textures * tex_head = NULL;
fields * field_head = NULL;

void load_config(char* filename) {
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int linetype = 0;
    int linenum = 0;
    int tex_tag_len = strlen(TEX_TAG);
    int field_tag_len = strlen(FIELD_TAG);
    textures * tex_next;
    textures * tex_last;
    fields * field_next;
    fields * field_last;

    fp = fopen(filename, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {
        if(!line || line[0] == COMMENT_CHAR || (linetype != 2 && strisblank(line))) {
            continue;
        }
        
        if(!strncmp( line, TEX_TAG, tex_tag_len)) {
            printf("Texture Mode:\n");
            linetype = 1;
            linenum = 0;
            continue;
        } else if(!strncmp(line, FIELD_TAG, field_tag_len)) {
            printf("Field Mode:\n");
            linetype = 2;
            linenum = 0;
            continue;
        }
        if(linetype == 1) {
            printf("Reading texture\n");
            if(linenum == 0) {
                printf("Texture 1\n");
                tex_next = (textures *) malloc(sizeof(textures));
                if(!tex_next) {
                    printf("Error allocating texture replacement\n");
                    continue;
                }
                //tex_next->from = malloc(20);
                strncpy(tex_next->from,line,min(20,strlen(line) - 1));
                tex_next->next = NULL;
                linenum = 1;
            } else {
                printf("Texture 2\n");
                //tex_next->to = malloc(20);
                strncpy(tex_next->to, line,min(20,strlen(line) - 1));
                if(!tex_head) {
                    tex_head = tex_next;
                } else {
                    tex_last->next = tex_next;
                }
                tex_last = tex_next;
                linenum = 0;
            }
        } else if(linetype == 2) {
            printf("Reading field\n");
            if(linenum == 0) {
                printf("Field 1\n");
                field_next = (fields *) malloc(sizeof(fields));
                if(!field_next) {
                    printf("Error allocating field\n");
                    continue;
                }
                strncpy(field_next->entname,line,min(100,strlen(line) - 1));
                field_next->next = NULL;
                linenum = 1;
            } else if(linenum == 1) {
                printf("Field 2\n");
                if(!field_next) {
                    printf("Error allocating field\n");
                    continue;
                }
                strncpy(field_next->fieldname,line,min(100,strlen(line) - 1));
                field_next->next = NULL;
                linenum = 2;
            } else if(linenum == 2) {
                printf("Field 2\n");
                if(!field_next) {
                    printf("Error allocating field\n");
                    continue;
                }
                strncpy(field_next->from,line,min(200,strlen(line) - 1));
                field_next->next = NULL;
                linenum = 3;
            } else {
                printf("Field 4\n");
                strncpy(field_next->to, line,min(200,strlen(line) - 1));
                if(!field_head) {
                    field_head = field_next;
                } else {
                    field_last->next = field_next;
                }
                field_last = field_next;
                linenum = 0;
            }
        }
        printf("Retrieved line of length %zu:\n", read);
        printf("%s\n", line);
    }

    fclose(fp);
    if (line)
        free(line);
    printf("Load Config finished\n");
}

void print_usage() {
    printf("Map Mirror %s\n", VERSION);
    printf("Usage: mapmirror [options] <mapfile.map>\n");
    printf("\t-h | --help  Print this info and exit\n");
    printf("\t-c <file>    Config file to use (default: mapmirror.conf)\n");
    printf("\t-o <file>    Output file to write (default: out.map)\n");
}

int main(int argc, char** argv) {
    char * config_file = "mapmirror.conf";
    char * map_file = "a.map";
    char * output_file = "out.map";
    int i = 0;
    for (i = 1; i < argc; ++i) {
        //printf("Arg %i: %s\n", i, argv[i]);
        if(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
            print_usage();
            exit(0);
        }
        if(!strcmp(argv[i], "-c")) {
            if(argc <= i + 1) {
                printf("Error: %s: Missing argument\n", argv[i]);
                print_usage();
                exit(EXIT_FAILURE);
            }
            config_file = (char *) malloc(sizeof(char) * strlen(argv[i+1]));
            strcpy(config_file, argv[++i]);
            continue;
        }
        if(!strcmp(argv[i], "-o")) {
            if(argc <= i + 1) {
                printf("Error: %s: Missing argument\n", argv[i]);
                print_usage();
                exit(EXIT_FAILURE);
            }
            output_file = (char *) malloc(sizeof(char) * strlen(argv[i+1]));
            strcpy(output_file, argv[++i]);
            continue;
        }
        map_file = (char *) malloc(sizeof(char) * strlen(argv[i]));
        strcpy(map_file, argv[i]);
    }
    printf("Loading config: %s\n", config_file);
    printf("Input map: %s\n", map_file);
    printf("Output map: %s\n", output_file);
    load_config(config_file);
    
    printf("Textures defined:\n");
    textures * t;
    t = tex_head;
    for (t = tex_head; t != NULL; t = t->next) {
        printf("Texture replacement: %s => %s\n", t->from, t->to);
    }
    fields * f;
    f = field_head;
    for (f = field_head; f != NULL; f = f->next) {
        printf("Field replacement: %s.%s: %s => %s\n", f->entname, f->fieldname, f->from, f->to);
    }

    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    FILE * fpout;
    int linetype = 0;

    fp = fopen(map_file, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    fpout = fopen(output_file, "w");
    if (fpout == NULL)
        exit(EXIT_FAILURE);

    ent_field * entbuffer[MAX_FIELDS];
    char * classname = NULL;
    //char * fieldname = NULL;
    //char * fieldvalue = NULL;
    brush_face * bf;
    char * tmp;
    char * tmp2;
    int fieldnum = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        //printf("Retrieved line of length %zu:\n", read);
        //printf("%s", line);
        if(!line) continue;
        if(line[0] == '(') {
            //brush line
            bf = (brush_face *) malloc(sizeof(brush_face));
            tmp2 = line;
            //fieldnum = 0;
            for (i = 0; tmp = strstr(tmp2, ")"); ++i) {
                tmp2 = tmp + 1;
                //fieldnum += tmp;
            }
            printf("Tmp2 now %s, i = %li\n", tmp2, (tmp2 - line));
            bf->prefix = (char *) malloc(sizeof(char) * (tmp2 - line));
            strncpy(bf->prefix, line, (tmp2 - line));
            tmp = strstr(tmp2 + 1, " ");
            bf->texture = (char *) malloc(sizeof(char) * (tmp2 - line - 1));
            strncpy(bf->texture, tmp2 + 1, (tmp - tmp2 - 1));
            bf->suffix = (char *) malloc(sizeof(char) * (tmp2 - line - 2));
            strncpy(bf->suffix, tmp + 1, strlen(tmp) - 2);
            printf("Brush Face: '%s':'%s':'%s'\n", bf->prefix, bf->texture, bf->suffix);
            t = tex_head;
            for (t = tex_head; t != NULL; t = t->next) {
                if(!strcmp(t->from, bf->texture)) {
                    bf->texture = (char *) malloc(sizeof(char) * strlen(t->to));
                    strcpy(bf->texture, t->to);
                }
            }
            fprintf(fpout, "%s %s %s\n", bf->prefix, bf->texture, bf->suffix);
            continue;
        } else if(linetype && line[0] == '"') {
            //entity line
            entbuffer[fieldnum] = (ent_field *) malloc(sizeof(ent_field));
            tmp = strstr(line + 1, "\"");
            entbuffer[fieldnum]->fieldname = (char *) malloc(sizeof(char) * (tmp - line - 1));
            strncpy(entbuffer[fieldnum]->fieldname, line + 1, (tmp - line - 1));
            entbuffer[fieldnum]->fieldvalue = (char *) malloc(sizeof(char) * (strlen(tmp) - 5));
            strncpy(entbuffer[fieldnum]->fieldvalue, tmp + 3, (strlen(tmp) - 5));
            printf("Entity: %s:%s\n", entbuffer[fieldnum]->fieldname, entbuffer[fieldnum]->fieldvalue);
            if(!strcmp(entbuffer[fieldnum]->fieldname, "classname")) {
                classname = (char *) malloc(sizeof(char) * strlen(entbuffer[fieldnum]->fieldvalue));
                strcpy(classname, entbuffer[fieldnum]->fieldvalue);
                printf("Setting classname to %s\n", classname);
            }
            fieldnum++;
            continue;
        } else if(line[0] == '{') {
            //object start
            linetype = 1;
            entbuffer[0] = NULL;
            fieldnum = 0;
            classname = NULL;
            //fieldname = NULL;
            //fieldvalue = NULL;
        } else if(line[0] != '"' && linetype) {
            //entity end
            printf("Ending entity, current classname: %s\n", classname);
            if(classname && entbuffer) {
                f = field_head;
                for (f = field_head; f != NULL; f = f->next) {
                    printf("Searching config: %s\n", f->entname);
                    if(!strcmp(f->entname, classname)) {
                        printf("Config found, Searching fields: %s\n", f->entname);
                        for(i = 0; i < MAX_FIELDS; ++i) {
                            if(!entbuffer[i]) {
                                printf("Abandon fields at index %i\n", i);
                                break;
                            }
                            printf("Field: %s\n", entbuffer[i]->fieldname);
                            if(!strcmp(f->fieldname, entbuffer[i]->fieldname)) {
                                printf("Field found: %s\n", entbuffer[i]->fieldname);
                                //fieldname = entbuffer[i]->fieldname;
                                //fieldvalue = entbuffer[i]->fieldvalue;
                                if(strisblank(f->from) || !strcmp(f->from, entbuffer[i]->fieldvalue)) {
                                    printf("Overriding old: %s, from: %s, to: %s\n", entbuffer[i]->fieldvalue, f->from, f->to);
                                    entbuffer[i]->fieldvalue = (char *) malloc(sizeof(char) * strlen(f->to));
                                    strcpy(entbuffer[i]->fieldvalue, f->to);
                                }
                                break;
                            }
                        }
                        //break;
                    }
                }
            }
            printf("Done\n");
            if(entbuffer) {
                for(i = 0; i < min(MAX_FIELDS, fieldnum); ++i) {
                    if(entbuffer[i] == NULL) {
                        printf("Breaking on %i\n", i);
                        break;
                    }
                    printf("Testing %i: %s:%s\n", i, entbuffer[i]->fieldname, entbuffer[i]->fieldvalue);
                    fprintf(fpout, "\"%s\" \"%s\"\n", entbuffer[i]->fieldname, entbuffer[i]->fieldvalue);
                }
            }
            
            entbuffer[0] = NULL;
            fieldnum = 0;
            classname = NULL;
            //fieldname = NULL;
            //fieldvalue = NULL;
            //continue;
        }
        fprintf(fpout, "%s", line);
    }

    fclose(fp);
    fclose(fpout);
    if (line)
        free(line);
    exit(EXIT_SUCCESS);
}