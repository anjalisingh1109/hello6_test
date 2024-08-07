/*-----------------------------------------------------------------------------------+
|                         PT Communication Systems Pvt. Ltd.
+-----------------------------------------------------------------------------------*/

/*----------------------------- COPYRIGHT INFORMATION -------------------------------+
| This Program and its Contents are the property of PT Communication Systems Pvt. Ltd.
| or its subsidiaries. This  document contains confidential  proprietary  information,
| which is also protected as an unpublished  work under applicable Copyright Laws. The
| reproduction, distribution, utilization or the communication of this document or any
| part  thereof, without  express authorization is strictly prohibited. Offenders will
| be held liable for the payment of damages.
| © 2024, PT Communication Systems Pvt. Ltd. or its subsidiaries.
| All rights reserved. Company confidential.
+-----------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------+
| Module      : IPPIS-TFT DISPLAY
| File name   : main.c
| Description :
| Author      :
| Version     :
| Reviewer    :
| Review date :
+-----------------------------------------------------------------------------------*/
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <gst/gst.h>
#include <gst/video/videooverlay.h>

#define NUM_VIDEOS 1
#define NUM_IMAGES 1
#define NUM_TEXTS 1

typedef struct {
    GtkWidget *fixed;
    GtkWidget *video_areas[NUM_VIDEOS];
    GtkWidget *image_areas[NUM_IMAGES];
    GtkWidget *text_areas[NUM_TEXTS];
    GstElement *playbins[NUM_VIDEOS];
    int current_view;
    int current_video;
    int current_image;
    int current_text;
} AppWidgets;

const char *VIDEO_FILES[NUM_VIDEOS] = {
 //   "file:///home/badrilal/V14.mp4",
 //   "file:///home/badrilal/V4.mp4",
 //   "file:///home/badrilal/V14.mp4",
    "file:///home/badrilal/V4.mp4"


};

const char *IMAGE_FILES[NUM_IMAGES] = {
  //  "/home/badrilal/image.jpg",
  //  "/home/badrilal/image.jpg",
  //  "/home/badrilal/image.jpg",
    "/home/badrilal/image.jpg"

};

const char *TEXTS[NUM_TEXTS] = {
//    "Text view 1",
//    "Text view 2",
//    "Text view 3",
      "INDIAN RAILWAYS WISHES YOU HAPPY AND COMFORTABLE JOURNEY"

};

static gboolean update_label_text(gpointer data) {
    AppWidgets *widgets = (AppWidgets *)data;
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s (updated)", TEXTS[widgets->current_text]);
    gtk_label_set_text(GTK_LABEL(widgets->text_areas[widgets->current_text]), buffer);
    return TRUE;
}

static gboolean toggle_views(gpointer data) {
    AppWidgets *widgets = (AppWidgets *)data;

    // Hide all video, image, and text areas
    for (int i = 0; i < NUM_VIDEOS; i++) {
        gtk_widget_hide(widgets->video_areas[i]);
        gst_element_set_state(widgets->playbins[i], GST_STATE_PAUSED);
    }
    for (int i = 0; i < NUM_IMAGES; i++) {
        gtk_widget_hide(widgets->image_areas[i]);
    }
    for (int i = 0; i < NUM_TEXTS; i++) {
        gtk_widget_hide(widgets->text_areas[i]);
    }

    // Show the current view
    switch (widgets->current_view) {
        case 0: // Video
            gtk_widget_show(widgets->video_areas[widgets->current_video]);
            gst_element_set_state(widgets->playbins[widgets->current_video], GST_STATE_PLAYING);
            widgets->current_video = (widgets->current_video + 1) % NUM_VIDEOS;
            break;
        case 1: // Image
            gtk_widget_show(widgets->image_areas[widgets->current_image]);
            widgets->current_image = (widgets->current_image + 1) % NUM_IMAGES;
            break;
        case 2: // Text
            gtk_widget_show(widgets->text_areas[widgets->current_text]);
            widgets->current_text = (widgets->current_text + 1) % NUM_TEXTS;
            break;
    }

    widgets->current_view = (widgets->current_view + 1) % 3;

    return TRUE;
}

static void realize_cb(GtkWidget *widget, gpointer data) {
    int index = GPOINTER_TO_INT(data);
    AppWidgets *widgets = (AppWidgets *)g_object_get_data(G_OBJECT(widget), "appwidgets");
    GdkWindow *window = gtk_widget_get_window(widget);
    if (!gdk_window_ensure_native(window)) {
        g_error("Couldn't create native window needed for GstVideoOverlay!");
    }

    GstVideoOverlay *overlay = GST_VIDEO_OVERLAY(widgets->playbins[index]);
    gst_video_overlay_set_window_handle(overlay, GDK_WINDOW_XID(window));
}




// Function to apply CSS from a file to a widget
void apply_css(GtkWidget *widget, const gchar *css_file_path) {
    // Create a new CSS provider
    GtkCssProvider *provider = gtk_css_provider_new();
    // Get the screen of the widget
    GdkScreen *screen = gtk_widget_get_screen(widget);
    // Get the style context of the widget
    GtkStyleContext *context = gtk_widget_get_style_context(widget);

    // Load CSS from the specified file and apply it to the screen
    if (gtk_css_provider_load_from_path(provider, css_file_path, NULL)) {
        gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    } else {
        // Print a warning if CSS file loading fails
        g_warning("Failed to load CSS from file: %s", css_file_path);
    }

    // Unreference the provider to free memory
    g_object_unref(provider);
}



// Function to add a CSS class to a widget
void add_css_class(GtkWidget *widget, const gchar *css_class) 
{
    // Get the style context of the widget
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    // Add the specified CSS class to the widget's style context
    gtk_style_context_add_class(context, css_class);

}

static void activate(GtkApplication *app, gpointer user_data) {
    AppWidgets *widgets = g_new0(AppWidgets, 1);

    // Initialize GStreamer
    gst_init(NULL, NULL);

    // Create the main window
    GtkWidget *window = gtk_application_window_new(GTK_APPLICATION(app));
    gtk_window_set_title(GTK_WINDOW(window), "Multiple Videos, Images, and Texts Switcher");
    gtk_window_set_default_size(GTK_WINDOW(window), 1280, 720);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a fixed container
    widgets->fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), widgets->fixed);

    // Create video areas
    for (int i = 0; i < NUM_VIDEOS; i++) 
   {
        widgets->video_areas[i] = gtk_drawing_area_new();
        gtk_widget_set_size_request(widgets->video_areas[i], 640, 480);
        g_signal_connect(widgets->video_areas[i], "realize", G_CALLBACK(realize_cb), GINT_TO_POINTER(i));
        gtk_fixed_put(GTK_FIXED(widgets->fixed), widgets->video_areas[i], 0, 120);
        gtk_widget_hide(widgets->video_areas[i]);

        widgets->playbins[i] = gst_element_factory_make("playbin", NULL);
        g_object_set(widgets->playbins[i], "uri", VIDEO_FILES[i], NULL);
        g_object_set_data(G_OBJECT(widgets->video_areas[i]), "appwidgets", widgets);
    }

    // Create image areas
    for (int i = 0; i < NUM_IMAGES; i++) 
   {
        widgets->image_areas[i] = gtk_image_new_from_file(IMAGE_FILES[i]);
        gtk_fixed_put(GTK_FIXED(widgets->fixed), widgets->image_areas[i], 0, 120);
        gtk_widget_hide(widgets->image_areas[i]);
    }

    // Create text areas
    for (int i = 0; i < NUM_TEXTS; i++) 
    {
        widgets->text_areas[i] = gtk_label_new(TEXTS[i]);
        gtk_fixed_put(GTK_FIXED(widgets->fixed), widgets->text_areas[i], 0, 120);
        gtk_widget_hide(widgets->text_areas[i]);
        add_css_class(widgets->text_areas[i], "head1");
	apply_css(widgets->text_areas[i], "style.css");
    }

    // Initialize the views to start with the first video
    widgets->current_view = 0;
    widgets->current_video = 0;
    widgets->current_image = 0;
    widgets->current_text = 0;

    
    // Show all widgets
    gtk_widget_show_all(window);

    // Set the first video pipeline to playing state initially
    gst_element_set_state(widgets->playbins[widgets->current_video], GST_STATE_PLAYING);
    gtk_widget_show(widgets->video_areas[widgets->current_video]);

    // Set a timer to update the text view every second
    g_timeout_add_seconds(1, update_label_text, widgets);

    // Set a timer to toggle between video, image, and text every second
    g_timeout_add_seconds(1, toggle_views, widgets);
}

int main(int argc, char *argv[]) 
{
    GtkApplication *app;
    int status;
    // Initialize GTK application
    app = gtk_application_new("org.example.video_image_switcher", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
/* WORK AREA */
/*-----------------------------------------------------------------------------------+
| © 2024, PT Communication Systems Pvt. Ltd. or its subsidiaries.
| All rights reserved. Company confidential.
+-----------------------------------------------------------------------------------*/
