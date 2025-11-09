#ifndef DETECTOR_H
#define DETECTOR_H

typedef struct {
    double overall_score;
    double ast_similarity;
    double cfg_similarity;
    double dag_similarity;
    int total_nodes_1;
    int total_nodes_2;
    char verdict[256];
} PlagiarismResult;

PlagiarismResult detect_plagiarism(const char *code1, const char *code2);

#endif