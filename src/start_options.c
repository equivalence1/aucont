#include <start_options.h>

#include <stdlib.h>
#include <stdio.h>
#include <argp.h>
#include <list.h>

static
struct argp_option argp_options[] = {
    {"verbose",  'v', 0,         0,  "Produce verbose output",               0},
    {NULL,       'd', NULL,      0,  "Daemonize container",                  0},
    {"cpu",        1, "X%",      0,  "Max CPU percentage allowed to use",    0},
    {"net",        2, "IP",      0,  "IP address of this container",         0},
    {0}
};

static
void set_default_options(struct start_opts *opts)
{
    opts->cpu = 100;
    opts->detached = 0;
    opts->verbose = 0;
    opts->net = NULL;
    opts->image_path = NULL;
    opts->cmd = NULL;
    opts->cmd_args = (struct list_head *)malloc(sizeof(struct list_head));
    if (opts->cmd_args == NULL) {
        printf("No mem.");
        exit(EXIT_FAILURE);
    }
    INIT_LIST_HEAD(opts->cmd_args);
}

static
error_t parse_key_arg(char *arg, struct argp_state *state)
{
    struct start_opts *opts = (struct start_opts *)state->input;

    if (state->arg_num == 0) {
        opts->image_path = arg;
        return 0;
    }
    if (state->arg_num == 1) {
        opts->cmd = arg;
        return 0;
    }
   
    struct cmd_arg *cmd_arg = (struct cmd_arg *)malloc(sizeof(struct cmd_arg));
    if (cmd_arg == NULL)
        return -ENOMEM;
    cmd_arg->arg = arg;
    INIT_LIST_HEAD(&cmd_arg->args);
    list_add_tail(opts->cmd_args, &cmd_arg->args);

    return 0;
}

static
error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    struct start_opts *opts = (struct start_opts *)state->input;

    switch (key) {
        case 'd':
            opts->detached = 1;
            break;
        case 1:
            opts->cpu = atoi(arg);
            break;
        case 2:
            opts->net = arg;
            break;
        case 'v':
            opts->verbose = 1;
        case ARGP_KEY_ARG:
            return parse_key_arg(arg, state);
        break;
        default:
            break;
    }

    return 0;
}

const char *argp_program_version = "0.1";

static
struct argp argp = {
    .options = argp_options,
    .parser = parse_opt,
    .args_doc = NULL,
    .doc = NULL
};

void get_start_options(struct start_opts *opts, int argc, char *argv[])
{
    set_default_options(opts);
    argp_parse(&argp, argc, argv, 0, 0, opts);
}
