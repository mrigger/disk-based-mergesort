#include "merge.h"

void basic(void);
void multistep();
void replace_sel();
int presort(const void *, const void *);
void produce_super_run(int);
void merge_super_run(int);
void build_heap(int *, int);
void min_heapify(int *,int, int);
void replace_sel_run(int);
int prepare_presort_files();
void n_way_merge(int);

 
int main(int argc, char **argv)
{
        int method = 0;
        long file_size = 0;
        int no_of_keys = 0;
        struct timeval tm_start, tm_end, exec_tm;
        
        if(argc != 4)
        {
                printf("Usage: assn_3 merge-sort-method index-file sorted-index-file\n");
                printf("Exiting...!!\n");
                exit(1);
        }

        if(strcmp(argv[1], "--basic") == 0)
        {
                method = 0;
        }
        else if(strcmp(argv[1], "--multistep") == 0)
        {
                method = 1;
        }
        else if(strcmp(argv[1], "--replacement") == 0)
        {
                method = 2;
        }
        else
        {
                printf("Invalid mergesort method specified\n");
                printf("The value should be one of \n"
                        "1. --basic \n"
                        "2. --multistep \n"
                        "3. --replacement \n");
                printf("Exiting...!!\n");
                exit(1);
        }
        
        if( ( index_fp = fopen( argv[2], "r+b" ) ) == NULL ) 
        { 

#if DEBUG
                printf("Could not open the index file...Exiting!!!\n");
#endif          
                exit(1);       
        }
        
        fseek(index_fp, 0, SEEK_END);
        file_size = ftell(index_fp);
        no_of_keys = file_size/(sizeof(int));
        rewind(index_fp);

        strcpy(i_filename,argv[2]);
        strcpy(o_filename,argv[3]);

#if DEBUG
        printf("The number of keys in the file is %d\n", no_of_keys);
#endif         

        if(method == 0)  
        { 
                gettimeofday( &tm_start, NULL );
                basic(); 
                gettimeofday( &tm_end, NULL );
                
        }
        else if(method == 1)
        {
                gettimeofday( &tm_start, NULL );
                multistep();
                gettimeofday( &tm_end, NULL );
        }
        else if(method == 2)
        {
                gettimeofday( &tm_start, NULL );
                replace_sel();
                gettimeofday( &tm_end, NULL );
        }
        
        if( tm_end.tv_sec > tm_start.tv_sec )
        {
                timersub(&tm_end, &tm_start, &exec_tm);
        }
        else if( tm_end.tv_usec > tm_start.tv_usec )
        {
                timersub(&tm_end, &tm_start, &exec_tm);
        }
        else if( tm_end.tv_usec < tm_start.tv_usec )
        {
                timersub(&tm_start, &tm_end, &exec_tm);
        }
        printf( "Time: %ld.%06ld\n", exec_tm.tv_sec, exec_tm.tv_usec );
        fclose(index_fp);
}

int prepare_presort_files()
{
        int ret = 0;        
        int n = 0;
        int i; 
 
        while (!feof(index_fp))
        {
                ret = fread(in_buf, sizeof(int),IN_BUFF_SIZE, index_fp);

                if(ret == 0)
                {
                        break;
                }
                else if(ferror(index_fp))
                {
                        printf("Error reading\n");
                        break;
                }
                else
                {        
                        FILE *fp_temp;
                        char num[10];
                        sprintf(num, "%03d", n);
                        char file_name[100];
                        strcpy(file_name, i_filename);
                        strcat(file_name, ".");
                        strcat(file_name, num);
                        
                        qsort(in_buf,ret,sizeof(int),presort);
                        
                        fp_temp = fopen(file_name, "w+b");
                        fwrite( in_buf, sizeof( int ), ret, fp_temp );                
                        fclose(fp_temp);
                        n++;
                }
        }

        return n;
}


//Basic mergersort with n way merge of 'n' input buffer chunks
void basic()
{
#if DEBUG
        printf("Inside basic\n");
#endif        
        int n = 0;
        int ret = 0;
        int i;
       
        n = prepare_presort_files();
        if(n == 0)
        {
                printf("The input file is empty\n");
                return;        
        }
        n_way_merge(n);
}


void n_way_merge(int n)
{
#if DEBUG
        printf("n way merge called with n value %d\n", n);
#endif     
        int run_buff_size = 0;
        
        int i = 0;
        
        if(n == 1)
        {       
                FILE *op_fp_t;
                op_fp_t = fopen(o_filename, "wb+");
                int no_of_recs = 0;
                FILE *fp_temp_t;
                char num_t[10];
                sprintf(num_t, "%03d", 0);
                char file_name_t[100];
                strcpy(file_name_t, i_filename);
                strcat(file_name_t, ".");
                strcat(file_name_t, num_t);
                fp_temp_t = fopen(file_name_t, "rb+");
                int p = 0;
                no_of_recs = fread(in_buf, sizeof(int),IN_BUFF_SIZE, fp_temp_t);
                for(p=0;p<no_of_recs;p++)
                {
                        out_buf[p] = in_buf[p];
                }
                fwrite(out_buf, sizeof(int), no_of_recs, op_fp_t);
                while(1)
                {       
                        no_of_recs = fread(in_buf, sizeof(int),IN_BUFF_SIZE, fp_temp_t);
                        if(no_of_recs == 0 || (feof(fp_temp_t)))
                        {
                                break;
                        }
                        for(p=0;p<no_of_recs;p++)
                        {
                                out_buf[p] = in_buf[p];
                        }
                        fwrite(out_buf, sizeof(int), no_of_recs, op_fp_t);
                }
                if(no_of_recs > 0)
                {        
                        for(p=0;p<no_of_recs;p++)
                        {
                                out_buf[p] = in_buf[p];
                        }
                        fwrite(out_buf, sizeof(int), no_of_recs, op_fp_t);
                }
                fclose(op_fp_t);
                fclose(fp_temp_t);
                return;
        }
        
        run_buff_size = (IN_BUFF_SIZE)/(n);
        if(run_buff_size == 0)
                run_buff_size = 1;

        int k =0;
        for(k=0; k<IN_BUFF_SIZE; k++)
        {
                in_buf[k] = INT_MAX;
        }       

        int ret = 0;
        int total  = 0;
        int h = 0;
        int *subset_size;
        subset_size = (int *)malloc(sizeof(int) * n);
        
        for( i = 0; i < n; i++ )
        {
                FILE *fp_temp;
                char num[10];
                sprintf(num, "%03d", i);
                char file_name[100];
                strcpy(file_name, i_filename);
                strcat(file_name, ".");
                strcat(file_name, num);
                fp_temp = fopen(file_name, "r+b");

                //Read a part of each run file
                ret = fread(in_buf + (i*run_buff_size), sizeof(int), run_buff_size, fp_temp);
                subset_size[i] = ret;
                total = total + ret;
                fclose(fp_temp);
        }
                
        
        FILE *op_fp;
        op_fp = fopen(o_filename, "wb+");
        if(n == 1)
        {
                fwrite(in_buf, sizeof(int), run_buff_size, op_fp);
                fclose(op_fp);
                return;
        }
        
        int index = 0;
        int temp = 0;
        int smallest = INT_MAX;
        int row = 0;
        int col = 0;
        int run_count = 0;
        int op_index = 0;
        int *file_update_flag = (int *)malloc(sizeof(int)*n);
        int *file_exhaust_count = (int *)malloc(sizeof(int)*n);
        int *file_completed = (int *)malloc(sizeof(int)*n);
        int count_of_files_exhausted = 0;
        int all_files_completed = 0;
        int row_tmp, col_tmp;
        int *subset_id_track;
        subset_id_track = (int *)malloc(sizeof(int) * n);
        int *row_completed;
        row_completed = (int *)malloc(sizeof(int) * n);
        int total_no_of_keys = 0;
        int *file_open_count;
        file_open_count = (int *)malloc(sizeof(int) * n);
        
        for(i=0;i<n;i++)
        {
                file_update_flag[i] = 0;
                file_exhaust_count[i] = 0;
                file_completed[i] = 0;
                row_completed[i] = 0;
                subset_id_track[i] = 0;
                file_open_count[i] = 0;
        }
        
        int no_left = 0;
        int n_tmp;
        
        
        while(1)
        {
                row = col = 0;
                row_tmp = col_tmp = 0;
                smallest = INT_MAX;
                index = 0;
                //Read into in buff array the "chunks" of the file
                for(i = 0; i < n; i++)
                {
                        if(file_completed[i] == 1)
                        {
                                 continue;
                        }
                        if( ( ( in_buf[(i*run_buff_size) + subset_id_track[i]] < smallest) ) && (subset_id_track[i] < subset_size[i]) )
                        {
                                smallest = in_buf[(i*run_buff_size) + subset_id_track[i]];
                                index = i;
                        }
                }
                int c;
                int is_flag = 0;
                row = index;
                subset_id_track[index]++;
                if(subset_id_track[row] == (subset_size[row] ))
                {
                        row_completed[row] = 1;
                }        
      		//If any chunk in array completed, read in more data for that chunk
                if(row_completed[row] == 1)
                {
                        int a;
                        file_exhaust_count[row] += subset_size[row];
                        FILE *fp_tmp;
                        char number[10];
                        sprintf(number, "%03d", row);
                        char file_name[100];
                        strcpy(file_name, i_filename);
                        strcat(file_name, ".");
                        strcat(file_name, number);
                        fp_tmp = fopen(file_name, "rb+");
                        int e = fseek(fp_tmp, file_exhaust_count[row]*sizeof(int), SEEK_SET); //Seek to appropriate position in file
                        int fpos = ftell(fp_tmp);
                        if( (e!=0) || (ferror(fp_tmp) ) )
                        {
                                printf("fseek error\n");
                                count_of_files_exhausted++;
                                if(count_of_files_exhausted == n)
                                {
                                        all_files_completed = 1;     
                                }
                                file_completed[row] = 1;
                                fclose(fp_tmp);
                        }       
                        else
                        {  
                                int tmp = 0;
                                int rec  = fread(&in_buf[row*run_buff_size], sizeof(int), run_buff_size, fp_tmp); 

                                if(rec > 0) //Leave the flags as they are if no more elements
                                {
                                        row_completed[row] = 0;
                                        subset_size[row] = rec;
                                        subset_id_track[row] = 0;
                                        fclose(fp_tmp);
                                        file_open_count[row]++;
                                }
                                else if((rec == 0))
                                {
                                        count_of_files_exhausted++;
                                        if(count_of_files_exhausted == n)
                                        {
                                                all_files_completed = 1;    
                                        }
                                        file_completed[row] = 1; 
                                        fclose(fp_tmp);
                                }
                        }
                }
                if(smallest != INT_MAX)
                {
                        out_buf[op_index] = smallest;
                        op_index++;
                }

                if(op_index == OUT_BUFF_SIZE)
                {
                        total_no_of_keys += OUT_BUFF_SIZE;
                        fwrite( out_buf, sizeof( int ), op_index, op_fp);
                        op_index = 0;
                }   
                if(all_files_completed == 1)
                {
                        if(op_index !=0)
                        {
                                total_no_of_keys += op_index;
                                fwrite( out_buf, sizeof( int ), op_index, op_fp);
                        }
                        break;  
                }
        }
        fclose(op_fp);
}


//Multi step merge -- Run merge sort on multiple intermediate files
void multistep()
{

#if DEBUG
        printf("Inside multistep\n");
#endif        
        int n = 0;
        int ret = 0;
        int i;
        
        n = prepare_presort_files();

        if(n == 0)
        {
                printf("The input file is empty\n");
                return;        
        }
        
        FILE *op_fp;
        op_fp = fopen(o_filename, "wb+");
       
        
        int temp_run_size = n/(SUPER_RUN_NO);
        int last_run_size = n%(SUPER_RUN_NO);
        int run_count = temp_run_size;
        
        for(i=0;i<temp_run_size;i++)
        {
                produce_super_run(SUPER_RUN_NO);
        }
        if(last_run_size != 0)
        {
                produce_super_run(last_run_size);
                run_count++;
        }
#if DEBUG        
        printf("The total number of super run files is %d\n", run_count); 
#endif        
        merge_super_run(run_count);
}


//Merge sort and produce the intermediate super run files 
void produce_super_run(int n)
{

#if DEBUG
        printf("Inside produce_super_run\n");
#endif        
        int run_buff_size = 0;
        static int s_run_no = 0;
        int i = 0;
        
        run_buff_size = (IN_BUFF_SIZE)/(n);

#if DEBUG
        printf("The run buffer size is %d for step run %d\n", run_buff_size, s_run_no);
#endif        

        int k =0;
        for(k=0; k<IN_BUFF_SIZE; k++)
        {
                in_buf[k] = INT_MAX;
        }        
        
        int ret = 0;
        int total  = 0;
        int h = 0;
        int *subset_size;
        subset_size = (int *)malloc(sizeof(int) * n);


        for( i = s_run_no*(SUPER_RUN_NO); i < ((s_run_no*(SUPER_RUN_NO))+n); i++, h++ )
        {
                FILE *fp_temp;
                char num[10];
                sprintf(num, "%03d", i);
                char file_name[100];
                strcpy(file_name, i_filename);
                strcat(file_name, ".");
                strcat(file_name, num);
                fp_temp = fopen(file_name, "r+b");

                //Read a part of each run file
                ret = fread(in_buf + h*run_buff_size, sizeof(int), run_buff_size, fp_temp);
                subset_size[h] = ret;
                total = total + ret;
                fclose(fp_temp);
        }
                

        FILE *op_fp;
        char o_num[10];
        sprintf(o_num, "%03d", s_run_no);
        char o_file_name[100];
        strcpy(o_file_name, i_filename);
        strcat(o_file_name, ".super.");
        strcat(o_file_name, o_num);
        op_fp = fopen(o_file_name, "wb+");
        if(n == 1)
        {
                fwrite(in_buf, sizeof(int), total, op_fp);
                fclose(op_fp);
                return;
        }

        int index = 0;
        int temp = 0;
        int smallest = INT_MAX;
        int row = 0;
        int col = 0;
        int run_count = 0;
        int op_index = 0;
        int *file_update_flag = (int *)malloc(sizeof(int)*n);
        int *file_exhaust_count = (int *)malloc(sizeof(int)*n);
        int *file_completed = (int *)malloc(sizeof(int)*n);
        int count_of_files_exhausted = 0;
        int all_files_completed = 0;
        int row_tmp, col_tmp;
        int *subset_id_track;
        subset_id_track = (int *)malloc(sizeof(int) * n);
        int *row_completed;
        row_completed = (int *)malloc(sizeof(int) * n);
        for(i=0;i<n;i++)
        {
                file_update_flag[i] = 0;
                file_exhaust_count[i] = 0;
                file_completed[i] = 0;
                row_completed[i] = 0;
                subset_id_track[i] = 0;
        }
        
        int no_left = 0;
        int n_tmp;
        
        while(1)
        {
                row = col = 0;
                row_tmp = col_tmp = 0;
                smallest = INT_MAX;
                for(i = 0; i < n; i++)
                {
                        if(file_completed[i] == 1)
                        {
                                continue;
                        }
                        if( ( ( in_buf[(i*run_buff_size) + subset_id_track[i]] < smallest) ) && (subset_id_track[i] < subset_size[i]) )
                        {
                                smallest = in_buf[(i*run_buff_size) + subset_id_track[i]];
                                index = i;
                        }
                }
                int c;
                int is_flag = 0;
                subset_id_track[index]++;
                row = index;
                
                if(subset_id_track[row] == (subset_size[row]))
                {
                        row_completed[row] = 1;
                }        
      
                if(row_completed[row] == 1)
                {
                        int a;
                        file_exhaust_count[row] += subset_size[row];
                        FILE *fp_tmp;
                        char number[10];
                        sprintf(number, "%03d", row+s_run_no*SUPER_RUN_NO);
                        char file_name[100];
                        strcpy(file_name, i_filename);
                        strcat(file_name, ".");
                        strcat(file_name, number);
                        fp_tmp = fopen(file_name, "r+b");
                        int e = fseek(fp_tmp, file_exhaust_count[row]*sizeof(int), SEEK_SET); //Seek to appropriate position in file
                        if((e!=0) || (ferror(fp_tmp)))
                        {
                                count_of_files_exhausted++;
                                if(count_of_files_exhausted == n)
                                {
                                        all_files_completed = 1;     
                                }
                                fclose(fp_tmp);
                        }       
                        else
                        {  
                                int tmp = 0;
                                int rec  = fread(&in_buf[row*run_buff_size], sizeof(int), run_buff_size, fp_tmp); 

                                if(rec > 0) //Leave the flags as they are if no more elements
                                {
                                        row_completed[row] = 0;
                                        subset_size[row] = rec;
                                        subset_id_track[row] = 0;
                                        fclose(fp_tmp);
                                }
                                else if((rec == 0))
                                {
                                        count_of_files_exhausted++;
                                        if(count_of_files_exhausted == n)
                                        {
                                                all_files_completed = 1;    
                                        }
                                        file_completed[row] = 1; 
                                        fclose(fp_tmp);
                                }
                        }
                }
                if(smallest != INT_MAX)
                {
                        out_buf[op_index] = smallest;
                        op_index++;
                }
                if(op_index == OUT_BUFF_SIZE)
                {
                        fwrite( out_buf, sizeof( int ), op_index, op_fp);
                        op_index = 0;
                }   
                if(all_files_completed == 1)
                {
                        if(op_index !=0)
                        {
                                fwrite( out_buf, sizeof( int ), op_index, op_fp);
                        }
                        s_run_no++;
                        break;  
                }
             
        }
        fclose(op_fp);
}


//Merge all the super run files to produce the final sorted file 
void merge_super_run(int n)
{
#if DEBUG        
        printf("Inside merge_super_run\n");
#endif        
        int run_buff_size = 0;
        
        int i = 0;
        
        if(n == 1)
        {
                FILE *op_fp_t;
                op_fp_t = fopen(o_filename, "wb+");
                int no_of_recs = 0;
                FILE *fp_temp_t;
                char num_t[10];
                sprintf(num_t, "%03d", 0);
                char file_name_t[100];
                strcpy(file_name_t, i_filename);
                strcat(file_name_t,".super.");
                strcat(file_name_t, num_t);
                fp_temp_t = fopen(file_name_t, "rb+");
                int p = 0;
                no_of_recs = fread(in_buf, sizeof(int),IN_BUFF_SIZE, fp_temp_t);
                
                for(p=0;p<no_of_recs;p++)
                {
                        out_buf[p] = in_buf[p];
                }
                fwrite(out_buf, sizeof(int), no_of_recs, op_fp_t);
                while(1)
                {       
                        no_of_recs = fread(in_buf, sizeof(int),IN_BUFF_SIZE, fp_temp_t);
                        if(no_of_recs == 0 || (feof(fp_temp_t)))
                                break;
                        for(p=0;p<no_of_recs;p++)
                        {
                                out_buf[p] = in_buf[p];
                        }
                        fwrite(out_buf, sizeof(int), no_of_recs, op_fp_t);
                }
                if(no_of_recs > 0)
                {        
                        for(p=0;p<no_of_recs;p++)
                        {
                                out_buf[p] = in_buf[p];
                        }
                        fwrite(out_buf, sizeof(int), no_of_recs, op_fp_t);
                }
                fclose(op_fp_t);
                fclose(fp_temp_t);
                return;
        }
        
        run_buff_size = (IN_BUFF_SIZE)/(n);
        if(run_buff_size == 0)
                run_buff_size = 1;


        int k =0;
        for(k=0; k<IN_BUFF_SIZE; k++)
        {
                in_buf[k] = INT_MAX;
        }        

        int ret = 0;
        int total  = 0;
        int h = 0;
        int *subset_size;
        subset_size = (int *)malloc(sizeof(int) * n);
        
        for( i = 0; i < n; i++ )
        {
                FILE *fp_temp;
                char num[10];
                sprintf(num, "%03d", i);
                char file_name[100];
                strcpy(file_name, i_filename);
                strcat(file_name,".super.");
                strcat(file_name, num);
                fp_temp = fopen(file_name, "r+b");

                //Read a part of each run file
                ret = fread(in_buf + i*run_buff_size, sizeof(int), run_buff_size, fp_temp);
                subset_size[i] = ret;
                total = total + ret;
                fclose(fp_temp);
        }
                        
        FILE *op_fp;
        op_fp = fopen(o_filename, "wb+");
        if(n == 1)
        {
                fwrite(in_buf, sizeof(int), total, op_fp);
                fclose(op_fp);
                return;
        }

        int index = 0;
        int temp = 0;
        int smallest = INT_MAX;
        int row = 0;
        int col = 0;
        int run_count = 0;
        int op_index = 0;
        int *file_update_flag = (int *)malloc(sizeof(int)*n);
        int *file_exhaust_count = (int *)malloc(sizeof(int)*n);
        int *file_completed = (int *)malloc(sizeof(int)*n);
        int count_of_files_exhausted = 0;
        int all_files_completed = 0;
        int row_tmp, col_tmp;
        int *subset_id_track;
        subset_id_track = (int *)malloc(sizeof(int) * n);
        int *row_completed;
        row_completed = (int *)malloc(sizeof(int) * n);

        for(i=0;i<n;i++)
        {
                file_update_flag[i] = 0;
                file_exhaust_count[i] = 0;
                file_completed[i] = 0;
                row_completed[i] = 0;
                subset_id_track[i] = 0;
        }
        
        int no_left = 0;
        int n_tmp;
        
        while(1)
        {
                row = col = 0;
                row_tmp = col_tmp = 0;
                smallest = INT_MAX;
                for(i = 0; i < n; i++)
                {
                        if(file_completed[i] == 1)
                        {
                                continue;
                        }
                        if( (( in_buf[(i*run_buff_size) + subset_id_track[i]] < smallest) )  && (subset_id_track[i] < subset_size[i] ) )
                        {
                                smallest = in_buf[(i*run_buff_size) + subset_id_track[i]];
                                index = i;
                        }
                }
                int c;
                int is_flag = 0;
                subset_id_track[index]++;
                row = index;
                
                if(subset_id_track[row] == (subset_size[row]))
                {
                        row_completed[row] = 1;
                }        
      
                if(row_completed[row] == 1)
                {
                        int a;
                        file_exhaust_count[row] += subset_size[row];
                        FILE *fp_tmp;
                        char number[10];
                        sprintf(number, "%03d", row);
                        char file_name[100];
                        strcpy(file_name, i_filename);
                        strcat(file_name, ".super.");
                        strcat(file_name, number);
                        fp_tmp = fopen(file_name, "r+b");
                        
                        int e = fseek(fp_tmp, file_exhaust_count[row]*sizeof(int), SEEK_SET); //Seek to appropriate position in file
                        if((e!=0) || (ferror(fp_tmp)))
                        {
                                count_of_files_exhausted++;
                                if(count_of_files_exhausted == n)
                                {
                                        all_files_completed = 1;     
                                }
                                fclose(fp_tmp);
                        }       
                        else
                        {  
                                int tmp = 0;
                                int rec  = fread(&in_buf[row*run_buff_size], sizeof(int), run_buff_size, fp_tmp); 

                                if(rec > 0) //Leave the flags as they are if no more elements
                                {
                                        row_completed[row] = 0;
                                        subset_size[row] = rec;
                                        subset_id_track[row] = 0;
                                        fclose(fp_tmp);
                                }
                                else if((rec == 0))
                                {
                                        count_of_files_exhausted++;
                                        if(count_of_files_exhausted == n)
                                        {
                                                all_files_completed = 1;    
                                        }
                                        file_completed[row] = 1; 
                                        fclose(fp_tmp);
                                }
                        }
                }
                if(smallest != INT_MAX)
                {
                        out_buf[op_index] = smallest;
                        op_index++;
                }
                if(op_index == OUT_BUFF_SIZE)
                {
                        fwrite( out_buf, sizeof( int ), op_index, op_fp);
                        op_index = 0;
                }   
                if(all_files_completed == 1)
                {
                        if(op_index !=0)
                                fwrite( out_buf, sizeof( int ), op_index, op_fp);
                        break;  
                }
             
        }
        fclose(op_fp);
}

//Replacement selection technique
void replace_sel()
{
#if DEBUG
        printf("Inside replace_sel\n");
#endif     
        int min_heap[750];
        int buffer[250];
        int i,j,k;
        int ret = 0;
        int total  = 0;
        int prim_heap_size = 0;
        int heap_min = 0;
        int file_num  = 0;
        int top = 0;
        int count_of_keys = 0;
        int op_index = 0;
        int sec_heap_size = 0;
        int flag_over = 0;
        int is_inp_over = 0;
        int total_c = 0;
        int prim_heap_size_tmp = 0;


        ret = fread(min_heap, sizeof(int), 750, index_fp);
        
        if(ret == 0)
        {
                printf("The input file is empty\n");
                return;        
        }
        
        if(ret < 750)//Just sort and write to out buffer
        {
                prim_heap_size = ret;
                FILE *op_fp_t;
                char inp_file[200];
                strcpy(inp_file, i_filename);
                strcat(inp_file, ".000");
                op_fp_t = fopen(inp_file,"wb+");
                
                build_heap(min_heap, prim_heap_size);
                while(prim_heap_size > 0)
                {
                        out_buf[op_index] = min_heap[0];
                        min_heap[0] = min_heap[prim_heap_size-1];
                        prim_heap_size--;
                        min_heapify(min_heap,1, prim_heap_size);
                        op_index++;
                }
                fwrite(out_buf, sizeof(int), op_index, op_fp_t);
                fclose(op_fp_t);
                file_num++;
                n_way_merge(file_num);
                return;
        }
        
        total = total + ret;
        prim_heap_size = 750;
        
        ret = fread(buffer, sizeof(int), 250, index_fp);
        total = total + ret;
        
        FILE *fp_temp;
        char num_tmp1[10];
        sprintf(num_tmp1, "%03d", file_num);
        char file_name_tmp1[200];
        strcpy(file_name_tmp1, i_filename);
        strcat(file_name_tmp1, ".");
        strcat(file_name_tmp1, num_tmp1);
        fp_temp = fopen(file_name_tmp1, "wb+");

	int g;
        if(feof(index_fp) && (ret == 0))
        {
                is_inp_over = 1;
        }
        file_num++;
        
        build_heap(min_heap, prim_heap_size);
        while(1)
        {
                heap_min = min_heap[0];
                out_buf[op_index] = heap_min;
                op_index++;
                if( (op_index == OUT_BUFF_SIZE) )
                {
                        fwrite( out_buf, sizeof( int ), OUT_BUFF_SIZE, fp_temp );                
                        total_c += OUT_BUFF_SIZE;
                        op_index = 0;
                }
                if(heap_min <= buffer[top])
                {
                        min_heap[0] = buffer[top];
                        top++;
                        min_heapify(min_heap,1, prim_heap_size);
                }
                else
                {
                        min_heap[0] = min_heap[prim_heap_size-1];
                        prim_heap_size--;
                        min_heapify(min_heap, 1, prim_heap_size);
                        min_heap[prim_heap_size] = buffer[top];
                        top++;
                        sec_heap_size++;
                }        
                
                
                if(sec_heap_size == 750) //Sec heap full
                {
                        fwrite( out_buf, sizeof( int ), op_index, fp_temp );        
                        total_c += op_index;
                        fclose(fp_temp);        
                        op_index = 0;
                        char num_tmp[10];
                        sprintf(num_tmp, "%03d", file_num);
                        char file_name_tmp[100];
                        strcpy(file_name_tmp, i_filename);
                        strcat(file_name_tmp, ".");
                        strcat(file_name_tmp, num_tmp);
                        fp_temp = fopen(file_name_tmp, "wb+");
                        
                        file_num++;
                        prim_heap_size = 750;
                        sec_heap_size = 0;
                        build_heap(min_heap, prim_heap_size);
                }       
                
                if( (top == ret) && (is_inp_over == 0))
                {
                        ret = fread(buffer, sizeof(int), 250, index_fp);
                        if( (ret == 0) )
                        {
                                break;
                        }
                        else
                        {
                                top = 0;
                        }
                }
        }
        if(op_index > 0 )
        {
                fwrite( out_buf, sizeof( int ), op_index, fp_temp );                
                total_c += op_index;
                op_index  = 0;
        }
        prim_heap_size_tmp = prim_heap_size;
        op_index = 0;
        
        if(prim_heap_size > 0) //Write leftover in primary heap to out buffer
        {
                build_heap(min_heap, prim_heap_size);
                while(prim_heap_size > 0)
                {
                        out_buf[op_index] = min_heap[0];
                        min_heap[0] = min_heap[prim_heap_size-1];
                        prim_heap_size--;
                        min_heapify(min_heap,1, prim_heap_size);
                        op_index++;
                }
                fwrite(out_buf, sizeof(int), op_index, fp_temp);
                total_c += prim_heap_size;
        }
        fclose(fp_temp);
        op_index = 0;
        sec_heap_size = 750 - prim_heap_size_tmp;
        
        build_heap(min_heap+prim_heap_size_tmp, sec_heap_size);
        
        while(sec_heap_size > 0) //Write leftover in secondary heap to out buffer
        {
                out_buf[op_index] = min_heap[prim_heap_size_tmp];
                min_heap[prim_heap_size_tmp] = min_heap[prim_heap_size_tmp + sec_heap_size - 1];
                sec_heap_size--;
                min_heapify(min_heap+prim_heap_size_tmp,1, sec_heap_size);
                op_index++;
        }
        
        char file_name_tmp2[100];
        char num_tmp2[10];
        sprintf(num_tmp2, "%03d", file_num);
        strcpy(file_name_tmp2, i_filename);
        strcat(file_name_tmp2, ".");
        strcat(file_name_tmp2, num_tmp2);
        fp_temp = fopen(file_name_tmp2, "w+b");
        total_c = 0;


        int rt = fwrite( out_buf, sizeof( int ),750-prim_heap_size_tmp, fp_temp );    
        total_c += (750-prim_heap_size_tmp);
        fclose(fp_temp);
        n_way_merge(file_num+1);            
} 

//Stock code for min heap build      
void build_heap(int *heap_array, int heap_size)
{
        
        int i = 0;
        for(i = (heap_size/2); i >=1; i-- )
        {
                min_heapify(heap_array, i, heap_size);
        }
}

//Stock code for min heapify
void min_heapify(int *heap_array, int i, int heap_length)
{
        
        int l, r;
        int smallest = i-1;
        
        l = 2*i-1;
        r = 2*i;
        
        if(l < heap_length && heap_array[l] < heap_array[i-1] )
        {
                smallest = l;
        }
        else
        {
                smallest = i-1;
        }
        if(r < heap_length && heap_array[r] < heap_array[smallest] )
        {
                smallest = r;
        }
        if(smallest != i-1)
        {
                int temp;
                temp = heap_array[smallest];
                heap_array[smallest] = heap_array[i-1];
                heap_array[i-1] = temp;
                min_heapify(heap_array, smallest+1, heap_length);
        }
}
 
        


int presort(const void *a, const void *b)
{
#if DEBUG
        printf("Inside presort\n");
#endif
        
        int *item_1 = (int *)a;
        int *item_2 = (int *)b;

        if( (*item_1) < (*item_2) )
        {
                return -1;
        }
        else if( (*item_1) > (*item_2) )
        {
                return 1;
        }
        else
        {
                return 0;
        }
}

