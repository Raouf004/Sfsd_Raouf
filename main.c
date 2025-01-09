#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RECORD_SIZE 100 // Taille maximale pour le contenu des enregistrements
#define MAX_BLOCKS 100 // Nombre maximum de blocs

// Structure representant un enregistrement
typedef struct {
    int record_ID; // Identifiant unique pour l'enregistrement
    char content[MAX_RECORD_SIZE]; // Contenu de l'enregistrement
} Record;

// Structure representant un bloc d'enregistrements
typedef struct {
    int is_empty; // Indicateur si le bloc est vide
    char file_name[50]; // Nom du fichier associe au bloc
    Record record_data; // Donnees de l'enregistrement dans le bloc
} Block;

// Structure representant un fichier contenant des enregistrements
typedef struct {
    char name[50]; // Nom du fichier
    int total_records; // Nombre total d'enregistrements dans le fichier
    Block *blocks[MAX_BLOCKS]; // Tableau de pointeurs vers les blocs
} File;

// Structure pour stocker des informations sur les metadonnees du fichier
typedef struct {
    char meta_file_name[50]; // Nom du fichier de metadonnees
    int block_count; // Nombre de blocs
    int record_count; // Nombre d'enregistrements
    int first_block_adr; // Adresse du premier bloc
} MetaInfo;

// Pointeur global pour simuler les blocs de disque
Block disk_blocks[MAX_BLOCKS];

// Prototypes de fonctions
void initialize_disk(int block_num);
void create_meta_info(File *file_obj, MetaInfo *meta_info);
void display_meta_info(const MetaInfo *meta_info);
void create_file(File *file_obj);
void insert_record(File *file_obj, const Record new_record);
int search_record(const File *file_obj, int record_ID);
void edit_record(File *file_obj, int record_ID, const char *new_content);
void delete_record(File *file_obj, int record_ID);
void sort_file(File *file_obj);
void defragment_file(File *file_obj);
void compact_memory();
void delete_file(File *file_obj);
void display_file(const File *file_obj);
void display_disk_status();
int count_records(const File *file_obj);
void search_by_content(const File *file_obj, const char *search_content);
void backup_file(const File *file_obj);
void restore_file(File *file_obj);

void initialize_disk(int block_num) {
    for (int i = 0; i < block_num; i++) {
        disk_blocks[i].is_empty = 1; // Marquer le bloc comme vide
    }
}

void create_meta_info(File *file_obj, MetaInfo *meta_info) {
    strcpy(meta_info->meta_file_name, file_obj->name);
    meta_info->block_count = file_obj->total_records;
    meta_info->record_count = file_obj->total_records;
    meta_info->first_block_adr = (file_obj->blocks[0] != NULL) ? 
                                    file_obj->blocks[0] - disk_blocks : -1;
}

void display_meta_info(const MetaInfo *meta_info) {
    printf("Nom du fichier : %s\n", meta_info->meta_file_name);
    printf("Nombre de blocs : %d\n", meta_info->block_count);
    printf("Nombre d'enregistrements : %d\n", meta_info->record_count);
    printf("Adresse du premier bloc : %d\n", meta_info->first_block_adr);
}

// void create_file(File *file_obj) {
//     printf("Entrez le nom du fichier : ");
//     scanf("%s", file_obj->name);
//     printf("Entrez le nombre d'enregistrements : ");
//     scanf("%d", &file_obj->total_records);
    
//     for (int i = 0; i < file_obj->total_records; i++) {
//         file_obj->blocks[i] = NULL;
//     }
// }






// -------------------------------------------------------------------------
void create_file(File *file_obj) {
    printf("Entrez le nom du fichier : ");
    scanf("%s", file_obj->name);

    // Ensure the user enters a valid number of records
    do {
        printf("Entrez le nombre d'enregistrements (doit etre positif) : ");
        scanf("%d", &file_obj->total_records);
        if (file_obj->total_records < 0) {
            printf("Erreur : Le nombre d'enregistrements doit etre positif.\n");
        }
    } while (file_obj->total_records < 0);
    
    for (int i = 0; i < file_obj->total_records; i++) {
        file_obj->blocks[i] = NULL;
    }
    printf("Fichier '%s' cree avec %d enregistrements.\n", file_obj->name, file_obj->total_records);
}
// -----------------------------------------------





void insert_record(File *file_obj, const Record new_record) {
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (disk_blocks[i].is_empty) {
            disk_blocks[i].is_empty = 0;
            disk_blocks[i].record_data = new_record;
            strcpy(disk_blocks[i].file_name, file_obj->name);
            file_obj->blocks[i] = &disk_blocks[i];
            return;
        }
    }
    printf("Erreur : Pas assez d'espace pour inserer un nouvel enregistrement.\n");
}

int search_record(const File *file_obj, int record_ID) {
    for (int i = 0; i < file_obj->total_records; i++) {
        if (file_obj->blocks[i] != NULL && 
            file_obj->blocks[i]->record_data.record_ID == record_ID) {
            return i; // Retourner l'index du bloc trouve
        }
    }
    return -1; // Retourner -1 si non trouve
}

void edit_record(File *file_obj, int record_ID, const char *new_content) {
    int block_index = search_record(file_obj, record_ID);
    if (block_index != -1) {
        strcpy(file_obj->blocks[block_index]->record_data.content, new_content);
        printf("L'enregistrement avec ID %d a ete mis a jour avec succes.\n", record_ID);
    } else {
        printf("Enregistrement non trouve pour modification.\n");
    }
}

void delete_record(File *file_obj, int record_ID) {
    int block_index = search_record(file_obj, record_ID);
    if (block_index != -1) {
        file_obj->blocks[block_index]->is_empty = 1;
        file_obj->blocks[block_index] = NULL;
        printf("L'enregistrement avec ID %d a ete supprime.\n", record_ID);
    } else {
        printf("Enregistrement non trouve pour suppression.\n");
    }
}

void sort_file(File *file_obj) {
    for (int i = 0; i < file_obj->total_records - 1; i++) {
        for (int j = i + 1; j < file_obj->total_records; j++) {
            if (file_obj->blocks[i] != NULL && file_obj->blocks[j] != NULL &&
                file_obj->blocks[i]->record_data.record_ID > 
                file_obj->blocks[j]->record_data.record_ID) {
                Record temp = file_obj->blocks[i]->record_data;
                file_obj->blocks[i]->record_data = file_obj->blocks[j]->record_data;
                file_obj->blocks[j]->record_data = temp;
            }
        }
    }
}

void defragment_file(File *file_obj) {
    int index = 0;
    for (int i = 0; i < file_obj->total_records; i++) {
        if (file_obj->blocks[i] != NULL) {
            if (index != i) {
                file_obj->blocks[index] = file_obj->blocks[i];
                file_obj->blocks[i] = NULL;
            }
            index++;
        }
    }
}

void compact_memory() {
    int compact_index = 0;
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (!disk_blocks[i].is_empty) {
            if (compact_index != i) {
                disk_blocks[compact_index] = disk_blocks[i];
                disk_blocks[i].is_empty = 1;
            }
            compact_index++;
        }
    }
    printf("Memoire compactee avec succes.\n");
}

void delete_file(File *file_obj) {
    for (int i = 0; i < file_obj->total_records; i++) {
        if (file_obj->blocks[i] != NULL) {
            file_obj->blocks[i]->is_empty = 1;
            file_obj->blocks[i] = NULL;
        }
    }
    printf("Fichier %s supprime.\n", file_obj->name);
}

void display_file(const File *file_obj) {
    printf("Nom du fichier : %s\n", file_obj->name);
    printf("Nombre total d'enregistrements : %d\n", file_obj->total_records);
    for (int i = 0; i < file_obj->total_records; i++) {
        if (file_obj->blocks[i] != NULL) {
            printf("ID : %d, Contenu : %s\n", 
                   file_obj->blocks[i]->record_data.record_ID,
                   file_obj->blocks[i]->record_data.content);
        }
    }
}

void display_disk_status() {
    printf("\netat du disque :\n");
    for (int i = 0; i < MAX_BLOCKS; i++) {
        printf("---------------------\n");
        printf("| ");
        if (disk_blocks[i].is_empty) {
            printf("Vide            ");
        } else {
            printf("%-8s ", disk_blocks[i].file_name);
        }
        printf(" |\n");
        printf("---------------------\n");
    }
}

// Nouveau : Compter le nombre d'enregistrements
int count_records(const File *file_obj) {
    int count = 0;
    for (int i = 0; i < file_obj->total_records; i++) {
        if (file_obj->blocks[i] != NULL) {
            count++;
        }
    }
    return count; // Retourner le nombre d'enregistrements
}

// Nouveau : Rechercher par contenu
void search_by_content(const File *file_obj, const char *search_content) {
    int found = 0;
    for (int i = 0; i < file_obj->total_records; i++) {
        if (file_obj->blocks[i] != NULL && 
            strstr(file_obj->blocks[i]->record_data.content, search_content) != NULL) {
            printf("Enregistrement trouve : ID : %d, Contenu : %s\n",
                   file_obj->blocks[i]->record_data.record_ID,
                   file_obj->blocks[i]->record_data.content);
            found = 1;
        }
    }
    if (!found) {
        printf("Aucun enregistrement trouve avec le contenu specifie.\n");
    }
}

// Nouveau : Sauvegarde du fichier
void backup_file(const File *file_obj) {
    char backup_name[50];
    sprintf(backup_name, "%s.bak", file_obj->name);
    FILE *backup_file = fopen(backup_name, "wb");
    if (backup_file) {
        fwrite(file_obj, sizeof(File), 1, backup_file);
        fclose(backup_file);
        printf("Sauvegarde creee avec succes : %s\n", backup_name);
    } else {
        printf("Erreur lors de la creation de la sauvegarde.\n");
    }
}

// Nouveau : Restaurer le fichier a partir d'une sauvegarde
void restore_file(File *file_obj) {
    char backup_name[50];
    sprintf(backup_name, "%s.bak", file_obj->name);
    FILE *backup_file = fopen(backup_name, "rb");
    if (backup_file) {
        fread(file_obj, sizeof(File), 1, backup_file);
        fclose(backup_file);
        printf("Fichier restaure avec succes a partir de %s\n", backup_name);
    } else {
        printf("Erreur lors de la restauration du fichier.\n");
    }
}

int main() {
    File main_file; // Declarer un objet fichier principal
    MetaInfo meta_info; // Declarer un objet d'informations sur les metadonnees
    int user_choice; // Variable pour le choix de l'utilisateur

    int total_blocks;
    printf("Entrez le nombre de blocs disponibles sur le disque : ");
    scanf("%d", &total_blocks);
    initialize_disk(total_blocks); // Initialiser le disque

    // Boucle principale du menu
    while (1) {
        printf("\n*********************   Menu :    ******************************\n");
        printf("1. Creer un fichier .\n");
        printf("2. Inserer un enregistrement .\n");
        printf("3. Rechercher un enregistrement par ID .\n");
        printf("4. Modifier un enregistrement .\n");
        printf("5. Trier les enregistrements .\n");
        printf("6. Supprimer un enregistrement .\n");
        printf("7. Afficher le fichier .\n");
        printf("8. Defragmenter le fichier .\n");
        printf("9. Compacter la memoire .\n");
        printf("10. Effacer la memoire .\n");
        printf("11. Supprimer un fichier .\n");
        printf("12. Afficher l'etat du disque .\n");
        printf("13. Compter les enregistrements .\n");
        printf("14. Rechercher par contenu .\n");
        printf("15. Sauvegarder le fichier .\n");
        printf("16. Restaurer le fichier .\n");
        printf("17. Quitter .\n");
        printf("*******************************************************************\n");
        printf("Choisissez une option : ");
        scanf("%d", &user_choice); // Obtenir le choix de l'utilisateur

        // Gerer les choix de l'utilisateur
        switch (user_choice) {
            case 1:
                create_file(&main_file); // Creer un fichier
                create_meta_info(&main_file, &meta_info); // Creer des metadonnees
                break;
            case 2: {
                Record new_record;
                printf("Entrez l'ID de l'enregistrement : ");
                scanf("%d", &new_record.record_ID); // Lire l'ID de l'enregistrement
                printf("Entrez le contenu de l'enregistrement : ");
                getchar(); // Effacer le saut de ligne
                fgets(new_record.content, MAX_RECORD_SIZE, stdin); // Lire le contenu de l'enregistrement
                new_record.content[strcspn(new_record.content, "\n")] = 0; // Supprimer le saut de ligne
                insert_record(&main_file, new_record); // Inserer l'enregistrement
                break;
            }
            case 3: {
                int record_ID;
                printf("Entrez l'ID de l'enregistrement a rechercher : ");
                scanf("%d", &record_ID); // Lire l'ID de l'enregistrement a rechercher
                int index = search_record(&main_file, record_ID);
                if (index != -1) {
                    printf("L'enregistrement a ete trouve dans le bloc %d.\n", index);
                } else {
                    printf("Enregistrement non trouve.\n");
                }
                break;
            }
            case 4: {
                int record_ID;
                char new_content[MAX_RECORD_SIZE];
                printf("Entrez l'ID de l'enregistrement a modifier : ");
                scanf("%d", &record_ID); // Lire l'ID de l'enregistrement a modifier
                printf("Entrez le nouveau contenu : ");
                getchar(); // Effacer le saut de ligne
                fgets(new_content, MAX_RECORD_SIZE, stdin); // Lire le nouveau contenu
                new_content[strcspn(new_content, "\n")] = 0; // Supprimer le saut de ligne
                edit_record(&main_file, record_ID, new_content); // Modifier l'enregistrement
                break;
            }
            case 5:
                sort_file(&main_file); // Trier les enregistrements
                printf("Fichier trie avec succes.\n");
                break;
            case 6: {
                int record_ID;
                printf("Entrez l'ID de l'enregistrement a supprimer : ");
                scanf("%d", &record_ID); // Lire l'ID de l'enregistrement a supprimer
                delete_record(&main_file, record_ID); // Supprimer l'enregistrement
                break;
            }
            case 7:
                display_file(&main_file); // Afficher le contenu du fichier
                break;
            case 8:
                defragment_file(&main_file); // Defragmenter le fichier
                break;
            case 9:
                compact_memory(); // Compacter la memoire
                break;
            case 10:
                for (int i = 0; i < total_blocks; i++) {
                    disk_blocks[i].is_empty = 1; // Effacer tous les blocs de disque
                }
                printf("Memoire du disque effacee.\n");
                break;
            case 11:
                delete_file(&main_file); // Supprimer le fichier
                break;
            case 12:
                display_disk_status(); // Afficher l'etat du disque
                break;
            case 13:
                printf("Nombre total d'enregistrements : %d\n", count_records(&main_file)); // Compter les enregistrements
                break;
            case 14: {
                char search_content[MAX_RECORD_SIZE];
                printf("Entrez le contenu a rechercher : ");
                getchar(); // Effacer le saut de ligne
                fgets(search_content, MAX_RECORD_SIZE, stdin); // Lire le contenu a rechercher
                search_content[strcspn(search_content, "\n")] = 0; // Supprimer le saut de ligne
                search_by_content(&main_file, search_content); // Rechercher par contenu
                break;
            }
            case 15:
                backup_file(&main_file); // Sauvegarder le fichier
                break;
            case 16:
                restore_file(&main_file); // Restaurer le fichier
                break;
            case 17:
                printf("Au revoir !\n"); // Message de sortie
                return 0; // Quitter le programme
            default:
                printf("Option invalide.\n"); // Gestion des options invalides
        }
    }
}