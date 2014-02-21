### Init Workspace  ###
rm(list = ls())
source("/Users/bulent/Documents/AWorkspace/iceace/IceaceModel1.0/outputs/scripts/LibIceace.R", echo = TRUE)
data_dir = "/Users/bulent/Documents/AWorkspace/iceace/IceaceModel1.0/outputs/data"
output_dir = "/Users/bulent/Documents/AWorkspace/iceace/IceaceModel1.0/outputs/plots/"
setwd(output_dir)

# Import Datasets #
data_household = 0
source("/Users/bulent/Documents/AWorkspace/iceace/IceaceModel1.0/outputs/scripts/importSimResults.R", echo = T)

# Plotting Mall, Real Estate, Government, Central Bank and Equity Fund Vars #
source("/Users/bulent/Documents/AWorkspace/iceace/IceaceModel1.0/outputs/scripts/plotAgentVars.R", echo = T)


# Plot Bank Vars #
bankA = 1637
bankB = 1638
source("/Users/bulent/Documents/AWorkspace/iceace/IceaceModel1.0/outputs/scripts/plotBanksVars.R", echo = T)

# Plot Firm Vars #
source("/Users/bulent/Documents/AWorkspace/iceace/IceaceModel1.0/outputs/scripts/plotFirmsVars.R", echo = T)


# Compute GDP #
betaval = "0.30"
source("/Users/bulent/Documents/AWorkspace/iceace/IceaceModel1.0/outputs/scripts/computeGDPs.R", echo = T)

# Compute Writeoff Cases #
niter = 3600
source("/Users/bulent/Documents/AWorkspace/iceace/IceaceModel1.0/outputs/scripts/computeWriteoffs.R", echo = T)

# Compute Crediting Frequencies #
source("/Users/bulent/Documents/AWorkspace/iceace/IceaceModel1.0/outputs/scripts/computeCreditings.R", echo = T)

# Plot Household Vars #
if (data_household){
	source("/Users/bulent/Documents/AWorkspace/iceace/IceaceModel1.0/outputs/scripts/plotHouseholdVars.R", echo = T)
	}

# Compute Housing Market Activities#
source("/Users/bulent/Documents/AWorkspace/iceace/IceaceModel1.0/outputs/scripts/computeHousing.R", echo = T)

source("/Users/bulent/Documents/AWorkspace/iceace/IceaceModel1.0/outputs/scripts/identityCheck.R", echo = T)

