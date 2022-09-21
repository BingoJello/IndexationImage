import csv


file_name = "matrice_distance_euclidian.csv"
output_file_name = "output_matrice_distance_euclidian.csv"

#file_name = "matrice_distance_bhattacharyya.csv"
#output_file_name = "output_matrice_distance_bhattacharyya.csv"

file = open(file_name)
output_file = open(output_file_name, "a")

rows = []
header = ""
indice = -1
indice_img = 1
output = []
csvreader = csv.reader(file)
output_file.write("img1_name,img2_name,distance_similarity")
for row in csvreader:
    if(indice == -1):
        header = row
    else :
        for distance in row:
            output_file.write("\n"+str(header[indice])+".jpg,"+str(indice_img)+".jpg,"+str(distance))
            indice_img+=1
    indice+=1
    indice_img = 1

output_file.close()
output_file.close()
