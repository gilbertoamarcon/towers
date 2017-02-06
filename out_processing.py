import csv


out_data = []
with open('out.csv', 'r',newline = "") as csvfile:
    reader = csv.reader(csvfile, delimiter=",")
    for row in reader:
        out_data.append(row)
        
        
h0_cpu_size = []
for beam_width in [5, 10, 15, 20, 25, 50, 100, 10000]:
    for problem_size in [4,5,6,7]:
        h0_cpu_size.append([0, problem_size, beam_width])
        cpu = 0
        for row in out_data:
            if row[0] == "0" and row[2] == str(problem_size) and row[1] == str(beam_width):
                cpu += float(row[-1]) / 20
        h0_cpu_size[-1].append(cpu)
            
h1_cpu_size = []
for beam_width in [5, 10, 15, 20, 25, 50, 100, 10000]:
    for problem_size in [4,5,6,7]:
        h1_cpu_size.append([1, problem_size, beam_width])
        cpu = 0
        for row in out_data:
            if row[0] == "1" and row[2] == str(problem_size) and row[1] == str(beam_width):
                cpu += float(row[-1]) / 20
        h1_cpu_size[-1].append(cpu)

h_cpu_size = h0_cpu_size + h1_cpu_size

with open('h_cpu_sizeB.csv',  'w',newline = "") as csvfile:
    out_writer = csv.writer(csvfile, delimiter=',')
    for row in h_cpu_size:
        out_writer.writerow(row)
        
    

        