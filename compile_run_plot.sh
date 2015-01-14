#### A sample sequence of model compilation, execution and analyses ####

### Compiling ###
# - Parsing the model: locate and run xparser with the model description.
../xparser-0.17.0/xparser model_iceace.xml

# - Compiling the model: Link to your Libmboard directory and compile the .c codes.
make LIBMBOARD_DIR=/Users/bulent/Documents/AWorkspace/iceace/libmbdir

### Running ###
# - Picking an initilialization:
cp ./initialization/iceace_model_v1.0.0.test.xml ./outputs/its/0.xml

# - Running for 15 years, outputting snapshots for each year (20 x 12 iteration = 1 year).
./main 3600 initialization/0 -f 240

### Cleaning up ###
#make vclean

# Plotting the results of single run:
Rscript outputs/scripts/singleRunAnals.R