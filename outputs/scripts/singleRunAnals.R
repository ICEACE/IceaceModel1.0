### Init Workspace  ###
rm(list = ls())

base_dir = "/Users/bulent/Documents/AWorkspace/iceace/IceaceModel1.0"
scripts_dir = paste(base_dir, "/", "outputs/scripts/", sep ="")
data_dir = paste(base_dir, "/", "outputs/data/", sep ="")
output_dir = paste(base_dir, "/", "outputs/plots/", sep ="")
setwd(output_dir)

sfile = paste(scripts_dir, "/", "LibIceace.R", sep ="")
source(sfile, echo = TRUE)



# Import Datasets #
data_household = 0
sfile = paste(scripts_dir, "/", "importSimResults.R", sep ="")
source(sfile, echo = TRUE)

# Plotting Mall, Real Estate, Government, Central Bank and Equity Fund Vars #
sfile = paste(scripts_dir, "/", "plotAgentVars.R", sep ="")
source(sfile, echo = TRUE)

# Plot Bank Vars #
bankA = 3267
bankB = 3268
sfile = paste(scripts_dir, "/", "plotBanksVars.R", sep ="")
source(sfile, echo = TRUE)

# Plot Firm Vars #
sfile = paste(scripts_dir, "/", "plotFirmsVars.R", sep ="")
source(sfile, echo = TRUE)

# Compute GDP #
betaval = "0.60, Share Holders' Ratio = 1.0"
sfile = paste(scripts_dir, "/", "computeGDPs.R", sep ="")
source(sfile, echo = TRUE)

# Compute Writeoff Cases #
niter = 4800
sfile = paste(scripts_dir, "/", "computeWriteoffs.R", sep ="")
source(sfile, echo = TRUE)

# Compute Crediting Frequencies #
sfile = paste(scripts_dir, "/", "computeCreditings.R", sep ="")
source(sfile, echo = TRUE)

# Plot Household Vars #
if (data_household){
	sfile = paste(scripts_dir, "/", "plotHouseholdVars.R", sep ="")
	source(sfile, echo = TRUE)
	}

# Compute Housing Market Activities#
sfile = paste(scripts_dir, "/", "computeHousing.R", sep ="")
source(sfile, echo = TRUE)


# Identity Check
sfile = paste(scripts_dir, "/", "identityCheck.R", sep ="")
source(sfile, echo = TRUE)
