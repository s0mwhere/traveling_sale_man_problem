#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "read_txt.h"
#include "graph.h"
#include "priority_queue.h"
#include "kruskal_fetch.h"
#include "kruskal_algorithm.h"
#include "christofiles_algorithm.h"

typedef struct PathSegment {
    int from;
    int to;
    int* nodes;
    int num_nodes;
} PathSegment;

typedef struct {
    AdjList* graph;
    int num_nodes;
    int* required;
    int num_required;
    linked_list tsp_solution;
    int solved;
    double scale_x;
    double scale_y;
    double offset_x;
    double offset_y;
    gboolean* node_selected;
    PathSegment* path_segments;
    int num_segments;
    int current_step;        // Bước hiện tại đang hiển thị
    int total_steps;         // Tổng số bước
} AppData;

PathSegment* load_path_segments(const char* filename, int* num_segments) {
    FILE* f = fopen(filename, "r");
    if (!f) return NULL;
    
    int count = 0;
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        count++;
    }
    rewind(f);
    
    PathSegment* segments = malloc(count * sizeof(PathSegment));
    *num_segments = 0;
    
    while (fgets(line, sizeof(line), f)) {
        PathSegment* seg = &segments[*num_segments];
        
        int from, to, weight;
        if (sscanf(line, "%d %d %d", &from, &to, &weight) == 3) {
            seg->from = from;
            seg->to = to;
            
            char* ptr = line;
            for (int i = 0; i < 3; i++) {
                while (*ptr && *ptr != ' ') ptr++;
                while (*ptr && *ptr == ' ') ptr++;
            }
            
            int node_count = 0;
            char temp[1024];
            strcpy(temp, ptr);
            char* token = strtok(temp, " \n");
            while (token) {
                node_count++;
                token = strtok(NULL, " \n");
            }
            
            seg->num_nodes = node_count;
            seg->nodes = malloc(node_count * sizeof(int));
            
            int idx = 0;
            token = strtok(ptr, " \n");
            while (token && idx < node_count) {
                seg->nodes[idx++] = atoi(token);
                token = strtok(NULL, " \n");
            }
            
            (*num_segments)++;
        }
    }
    
    fclose(f);
    return segments;
}

PathSegment* find_path_segment(PathSegment* segments, int num_segments, int from, int to) {
    for (int i = 0; i < num_segments; i++) {
        if ((segments[i].from == from && segments[i].to == to) ||
            (segments[i].from == to && segments[i].to == from)) {
            return &segments[i];
        }
    }
    return NULL;
}

static void draw_arrow(cairo_t *cr, double x1, double y1, double x2, double y2) {
    cairo_move_to(cr, x1, y1);
    cairo_line_to(cr, x2, y2);
    cairo_stroke(cr);
    
    double angle = atan2(y2 - y1, x2 - x1);
    double arrow_length = 12;
    double arrow_angle = G_PI / 6;
    
    double end_x = x2 - 10 * cos(angle);
    double end_y = y2 - 10 * sin(angle);
    
    cairo_move_to(cr, end_x, end_y);
    cairo_line_to(cr, 
        end_x - arrow_length * cos(angle - arrow_angle),
        end_y - arrow_length * sin(angle - arrow_angle));
    cairo_stroke(cr);
    
    cairo_move_to(cr, end_x, end_y);
    cairo_line_to(cr,
        end_x - arrow_length * cos(angle + arrow_angle),
        end_y - arrow_length * sin(angle + arrow_angle));
    cairo_stroke(cr);
}

static gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    AppData *data = (AppData*)user_data;
    
    if (!data->graph) return FALSE;
    
    guint width = gtk_widget_get_allocated_width(widget);
    guint height = gtk_widget_get_allocated_height(widget);
    
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);
    
    double min_x = data->graph[0].x, max_x = data->graph[0].x;
    double min_y = data->graph[0].y, max_y = data->graph[0].y;
    
    for (int i = 0; i < data->num_nodes; i++) {
        if (data->graph[i].x < min_x) min_x = data->graph[i].x;
        if (data->graph[i].x > max_x) max_x = data->graph[i].x;
        if (data->graph[i].y < min_y) min_y = data->graph[i].y;
        if (data->graph[i].y > max_y) max_y = data->graph[i].y;
    }
    
    double margin = 50;
    data->scale_x = (width - 2*margin) / (max_x - min_x + 1);
    data->scale_y = (height - 2*margin) / (max_y - min_y + 1);
    double scale = fmin(data->scale_x, data->scale_y);
    data->offset_x = margin + (width - 2*margin - (max_x - min_x) * scale) / 2 - min_x * scale;
    data->offset_y = margin + (height - 2*margin - (max_y - min_y) * scale) / 2 - min_y * scale;
    
    // Vẽ các edges (màu xám nhạt)
    cairo_set_source_rgb(cr, 0.7, 0.7, 0.7);
    cairo_set_line_width(cr, 1);
    
    for (int i = 0; i < data->num_nodes; i++) {
        double x1 = data->graph[i].x * scale + data->offset_x;
        double y1 = data->graph[i].y * scale + data->offset_y;
        
        for (int j = 0; j < data->graph[i].size; j++) {
            int to = data->graph[i].edges[j].to;
            if (to > i) {
                double x2 = data->graph[to].x * scale + data->offset_x;
                double y2 = data->graph[to].y * scale + data->offset_y;
                
                cairo_move_to(cr, x1, y1);
                cairo_line_to(cr, x2, y2);
                cairo_stroke(cr);
            }
        }
    }
    
    // Vẽ đường TSP - chỉ hiển thị bước hiện tại
    if (data->solved && data->tsp_solution && data->path_segments && data->current_step > 0) {
        cairo_set_source_rgb(cr, 0.2, 0.6, 1.0);
        cairo_set_line_width(cr, 3);
        
        // Tìm đến bước hiện tại
        linked_list current = data->tsp_solution;
        int step_count = 1;
        
        while (current && current->next) {
            if (step_count == data->current_step) {
                int v1 = current->vertex;
                int v2 = current->next->vertex;
                
                PathSegment* seg = find_path_segment(data->path_segments, data->num_segments, v1, v2);
                
                if (seg && seg->num_nodes > 0) {
                    gboolean reverse = (seg->nodes[0] != v1);
                    
                    // Vẽ path segment của bước này
                    for (int i = 0; i < seg->num_nodes - 1; i++) {
                        int idx1 = reverse ? (seg->num_nodes - 1 - i) : i;
                        int idx2 = reverse ? (seg->num_nodes - 2 - i) : (i + 1);
                        
                        int n1 = seg->nodes[idx1];
                        int n2 = seg->nodes[idx2];
                        
                        double x1 = data->graph[n1].x * scale + data->offset_x;
                        double y1 = data->graph[n1].y * scale + data->offset_y;
                        double x2 = data->graph[n2].x * scale + data->offset_x;
                        double y2 = data->graph[n2].y * scale + data->offset_y;
                        
                        draw_arrow(cr, x1, y1, x2, y2);
                    }
                }
                break; // Chỉ vẽ bước hiện tại rồi dừng
            }
            
            current = current->next;
            step_count++;
        }
    }
    
    // Vẽ các nodes
    for (int i = 0; i < data->num_nodes; i++) {
        double x = data->graph[i].x * scale + data->offset_x;
        double y = data->graph[i].y * scale + data->offset_y;
        
        if (data->node_selected[i]) {
            cairo_set_source_rgb(cr, 1.0, 0.4, 0.4);
        } else {
            cairo_set_source_rgb(cr, 0.8, 0.8, 0.8);
        }
        cairo_arc(cr, x, y, 8, 0, 2 * G_PI);
        cairo_fill(cr);
        
        if (data->node_selected[i]) {
            cairo_set_source_rgb(cr, 0.7, 0.1, 0.1);
        } else {
            cairo_set_source_rgb(cr, 0.4, 0.4, 0.4);
        }
        cairo_set_line_width(cr, 2);
        cairo_arc(cr, x, y, 8, 0, 2 * G_PI);
        cairo_stroke(cr);
        
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, 12);
        
        char label[10];
        sprintf(label, "%d", i);
        
        cairo_text_extents_t extents;
        cairo_text_extents(cr, label, &extents);
        cairo_move_to(cr, x - extents.width/2, y - 12);
        cairo_show_text(cr, label);
    }
    
    return FALSE;
}

static gboolean on_button_press(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
    AppData *data = (AppData*)user_data;
    
    if (!data->graph) return FALSE;
    
    double scale = fmin(data->scale_x, data->scale_y);
    
    int closest_node = -1;
    double min_dist = 15;
    
    for (int i = 0; i < data->num_nodes; i++) {
        double x = data->graph[i].x * scale + data->offset_x;
        double y = data->graph[i].y * scale + data->offset_y;
        
        double dist = sqrt(pow(event->x - x, 2) + pow(event->y - y, 2));
        
        if (dist < min_dist) {
            min_dist = dist;
            closest_node = i;
        }
    }
    
    if (closest_node != -1) {
        data->node_selected[closest_node] = !data->node_selected[closest_node];
        data->solved = 0;
        data->current_step = 0;
        gtk_widget_queue_draw(widget);
    }
    
    return TRUE;
}

// Callback cho nút "Next Step" - hiển thị bước tiếp theo
static void on_next_step_clicked(GtkWidget *widget, gpointer user_data) {
    AppData *data = (AppData*)user_data;
    
    if (!data->solved) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Vui lòng giải TSP trước!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    if (data->current_step < data->total_steps) {
        data->current_step++;
        
        GtkWidget *drawing_area = GTK_WIDGET(g_object_get_data(G_OBJECT(widget), "drawing_area"));
        gtk_widget_queue_draw(drawing_area);
        
        // Cập nhật label
        GtkWidget *step_label = GTK_WIDGET(g_object_get_data(G_OBJECT(widget), "step_label"));
        char label_text[100];
        sprintf(label_text, "Bước: %d / %d", data->current_step, data->total_steps);
        gtk_label_set_text(GTK_LABEL(step_label), label_text);
    }
}

// Callback cho nút "Reset" - reset về bước 0
static void on_reset_clicked(GtkWidget *widget, gpointer user_data) {
    AppData *data = (AppData*)user_data;
    
    if (!data->solved) return;
    
    data->current_step = 0;
    
    GtkWidget *drawing_area = GTK_WIDGET(g_object_get_data(G_OBJECT(widget), "drawing_area"));
    gtk_widget_queue_draw(drawing_area);
    
    GtkWidget *step_label = GTK_WIDGET(g_object_get_data(G_OBJECT(widget), "step_label"));
    char label_text[100];
    sprintf(label_text, "Bước: %d / %d", data->current_step, data->total_steps);
    gtk_label_set_text(GTK_LABEL(step_label), label_text);
}

static void on_solve_clicked(GtkWidget *widget, gpointer user_data) {
    AppData *data = (AppData*)user_data;
    
    if (!data->graph) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Vui lòng load file raw_graph.txt trước!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    int count = 0;
    for (int i = 0; i < data->num_nodes; i++) {
        if (data->node_selected[i]) count++;
    }
    
    if (count < 3) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Vui lòng chọn ít nhất 3 node để giải TSP!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    // Kiểm tra giới hạn số node (DP bitmask chỉ xử lý được tối đa ~20 nodes)
    if (count > 20) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Số node quá lớn! Vui lòng chọn tối đa 20 nodes để tránh tràn bộ nhớ.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    if (data->required) free(data->required);
    data->required = malloc(count * sizeof(int));
    data->num_required = count;
    
    int idx = 0;
    for (int i = 0; i < data->num_nodes; i++) {
        if (data->node_selected[i]) {
            data->required[idx++] = i;
        }
    }
    
    gtk_widget_set_sensitive(widget, FALSE);
    
    if (data->tsp_solution) {
        free_ll(data->tsp_solution);
        data->tsp_solution = NULL;
    }
    
    if (data->path_segments) {
        for (int i = 0; i < data->num_segments; i++) {
            free(data->path_segments[i].nodes);
        }
        free(data->path_segments);
        data->path_segments = NULL;
    }
    
    A_Star(data->graph, data->required, data->num_required, "A_star.txt");
    data->path_segments = load_path_segments("A_star.txt", &data->num_segments);
    
    perf_graph_ptr graph = load_perf_graph("A_star.txt");
    mst_ptr tree = kruskal(graph, data->required);
    data->tsp_solution = christofides(graph, tree, data->required, data->num_required);
    
    data->solved = 1;
    data->current_step = 0;
    
    // Đếm tổng số bước
    data->total_steps = 0;
    linked_list current = data->tsp_solution;
    while (current && current->next) {
        data->total_steps++;
        current = current->next;
    }
    
    int total_weight = 0;
    current = data->tsp_solution;
    while (current) {
        total_weight += current->weight;
        current = current->next;
    }
    
    char msg[256];
    sprintf(msg, "TSP đã được giải!\nSố node: %d\nTổng khoảng cách: %d\nTổng số bước: %d\n\nNhấn 'Next Step' để xem từng bước", 
            count, total_weight, data->total_steps);
    
    GtkWidget *dialog = gtk_message_dialog_new(NULL,
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "%s", msg);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    
    gtk_widget_set_sensitive(widget, TRUE);
    
    // Cập nhật label
    GtkWidget *step_label = GTK_WIDGET(g_object_get_data(G_OBJECT(widget), "step_label"));
    char label_text[100];
    sprintf(label_text, "Bước: %d / %d", data->current_step, data->total_steps);
    gtk_label_set_text(GTK_LABEL(step_label), label_text);
    
    GtkWidget *drawing_area = GTK_WIDGET(g_object_get_data(G_OBJECT(widget), "drawing_area"));
    gtk_widget_queue_draw(drawing_area);
}

static void on_load_clicked(GtkWidget *widget, gpointer user_data) {
    AppData *data = (AppData*)user_data;
    
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Chọn file raw_graph.txt",
        NULL,
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Open", GTK_RESPONSE_ACCEPT,
        NULL);
    
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        
        FILE *f = fopen(filename, "r");
        if (f) {
            fscanf(f, "%d", &data->num_nodes);
            fclose(f);
            
            data->graph = create_graph(data->num_nodes);
            load_Graph(data->graph, filename);
            
            if (data->node_selected) free(data->node_selected);
            data->node_selected = malloc(data->num_nodes * sizeof(gboolean));
            for (int i = 0; i < data->num_nodes; i++) {
                data->node_selected[i] = FALSE;
            }
            
            data->solved = 0;
            data->current_step = 0;
            data->total_steps = 0;
            
            if (data->tsp_solution) {
                free_ll(data->tsp_solution);
                data->tsp_solution = NULL;
            }
            
            GtkWidget *drawing_area = GTK_WIDGET(g_object_get_data(G_OBJECT(widget), "drawing_area"));
            gtk_widget_queue_draw(drawing_area);
        }
        
        g_free(filename);
    }
    
    gtk_widget_destroy(dialog);
}

static void on_select_all_clicked(GtkWidget *widget, gpointer user_data) {
    AppData *data = (AppData*)user_data;
    
    if (!data->graph) return;
    
    for (int i = 0; i < data->num_nodes; i++) {
        data->node_selected[i] = TRUE;
    }
    
    data->solved = 0;
    data->current_step = 0;
    GtkWidget *drawing_area = GTK_WIDGET(g_object_get_data(G_OBJECT(widget), "drawing_area"));
    gtk_widget_queue_draw(drawing_area);
}

static void on_clear_selection_clicked(GtkWidget *widget, gpointer user_data) {
    AppData *data = (AppData*)user_data;
    
    if (!data->graph) return;
    
    for (int i = 0; i < data->num_nodes; i++) {
        data->node_selected[i] = FALSE;
    }
    
    data->solved = 0;
    data->current_step = 0;
    GtkWidget *drawing_area = GTK_WIDGET(g_object_get_data(G_OBJECT(widget), "drawing_area"));
    gtk_widget_queue_draw(drawing_area);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    
    AppData *data = g_new0(AppData, 1);
    data->graph = NULL;
    data->solved = 0;
    data->tsp_solution = NULL;
    data->node_selected = NULL;
    data->path_segments = NULL;
    data->num_segments = 0;
    data->current_step = 0;
    data->total_steps = 0;
    
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "TSP Solver - Step by Step Visualizer");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    
    GtkWidget *toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 5);
    
    GtkWidget *load_btn = gtk_button_new_with_label("Load File");
    gtk_box_pack_start(GTK_BOX(toolbar), load_btn, FALSE, FALSE, 5);
    
    GtkWidget *select_all_btn = gtk_button_new_with_label("Chọn Tất Cả");
    gtk_box_pack_start(GTK_BOX(toolbar), select_all_btn, FALSE, FALSE, 5);
    
    GtkWidget *clear_btn = gtk_button_new_with_label("Bỏ Chọn Tất Cả");
    gtk_box_pack_start(GTK_BOX(toolbar), clear_btn, FALSE, FALSE, 5);
    
    GtkWidget *solve_btn = gtk_button_new_with_label("Solve TSP");
    gtk_box_pack_start(GTK_BOX(toolbar), solve_btn, FALSE, FALSE, 5);
    
    GtkWidget *next_step_btn = gtk_button_new_with_label("Next Step");
    gtk_box_pack_start(GTK_BOX(toolbar), next_step_btn, FALSE, FALSE, 5);
    
    GtkWidget *reset_btn = gtk_button_new_with_label("Reset");
    gtk_box_pack_start(GTK_BOX(toolbar), reset_btn, FALSE, FALSE, 5);
    
    GtkWidget *step_label = gtk_label_new("Bước: 0 / 0");
    gtk_box_pack_start(GTK_BOX(toolbar), step_label, FALSE, FALSE, 5);
    
    GtkWidget *label = gtk_label_new("Click vào các node để chọn/bỏ chọn (màu đỏ = được chọn). Sau khi Solve, nhấn Next Step để xem từng bước");
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 5);
    
    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_widget_add_events(drawing_area, GDK_BUTTON_PRESS_MASK);
    gtk_box_pack_start(GTK_BOX(vbox), drawing_area, TRUE, TRUE, 0);
    
    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw), data);
    g_signal_connect(drawing_area, "button-press-event", G_CALLBACK(on_button_press), data);
    g_signal_connect(load_btn, "clicked", G_CALLBACK(on_load_clicked), data);
    g_signal_connect(select_all_btn, "clicked", G_CALLBACK(on_select_all_clicked), data);
    g_signal_connect(clear_btn, "clicked", G_CALLBACK(on_clear_selection_clicked), data);
    g_signal_connect(solve_btn, "clicked", G_CALLBACK(on_solve_clicked), data);
    g_signal_connect(next_step_btn, "clicked", G_CALLBACK(on_next_step_clicked), data);
    g_signal_connect(reset_btn, "clicked", G_CALLBACK(on_reset_clicked), data);
    
    g_object_set_data(G_OBJECT(load_btn), "drawing_area", drawing_area);
    g_object_set_data(G_OBJECT(select_all_btn), "drawing_area", drawing_area);
    g_object_set_data(G_OBJECT(clear_btn), "drawing_area", drawing_area);
    g_object_set_data(G_OBJECT(solve_btn), "drawing_area", drawing_area);
    g_object_set_data(G_OBJECT(solve_btn), "step_label", step_label);
    g_object_set_data(G_OBJECT(next_step_btn), "drawing_area", drawing_area);
    g_object_set_data(G_OBJECT(next_step_btn), "step_label", step_label);
    g_object_set_data(G_OBJECT(reset_btn), "drawing_area", drawing_area);
    g_object_set_data(G_OBJECT(reset_btn), "step_label", step_label);
    
    gtk_widget_show_all(window);
    gtk_main();
    
    // Cleanup
    if (data->tsp_solution) free_ll(data->tsp_solution);
    if (data->required) free(data->required);
    if (data->node_selected) free(data->node_selected);
    if (data->path_segments) {
        for (int i = 0; i < data->num_segments; i++) {
            free(data->path_segments[i].nodes);
        }
        free(data->path_segments);
    }
    g_free(data);
    
    return 0;
}