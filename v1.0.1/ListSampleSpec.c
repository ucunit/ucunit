#include "System.h"

//#define UCUNIT_MODE_SILENT

#include "uCUnit-v1.0.h"
#include "uCSpec-v1.0.h"
#include "ListSample.h"

static void Specifications(void)
{
  DESCRIBE("list_create(sizeof(int))")

    list_t* l = list_create(sizeof(int));

    IT("should not return NULL")
      SHOULD_NOT_BE_NULL(l);
    IT_END

    IT("should create an empty list")
      SHOULD_EQ(0, list_size(l));
    IT_END

    IT("should return an error when out of memory") IT_END

    list_destroy(&l);

  DESCRIBE_END

  DESCRIBE("list_insert(list*, element*, index)")

    IT("should increment list size when inserting an element")
      list_t* l = list_create(sizeof(int));
      int element = 25;
      list_insert(l, &element, 0);
      SHOULD_EQ(1, list_size(l)+1); // provoke failure
      list_destroy(&l);
    IT_END

    IT("should be able to insert an element before another")
      int before = 7;
      int another = 8;
      list_t* l = list_create(sizeof(int));
      list_insert(l, &another, 0);
      list_insert(l, &before, 0);
      SHOULD_EQ(0, list_index(l, &before));
      list_destroy(&l);
    IT_END

    IT("should return an error when passed an invalid positive index")
      int element = 5;
      list_t* l = list_create(sizeof(int));
      SHOULD_EQ(-1, list_insert(l, &element, 1));
      list_destroy(&l);
    IT_END

    IT("should return an error when passed a negative index")
      int element = 5;
      list_t* l = list_create(sizeof(int));
      SHOULD_EQ(-1, list_insert(l, &element, -1));
      list_destroy(&l);
    IT_END

  DESCRIBE_END

  DESCRIBE("list_index(list*, element*)")

    IT("should be able to find an element")
      int element = 6;
      list_t* l = list_create(sizeof(int));
      list_insert(l, &element, 0);
      SHOULD_EQ(0, list_index(l, &element));
      list_destroy(&l);
    IT_END

    IT("should should return an error for a non-existent element")
      int elements[] = {4, 5};
      int non_existent = 7;
      list_t* l = list_create(sizeof(int));
      list_insert(l, elements, 0);
      list_insert(l, elements+1, 1);
      SHOULD_EQ(-1, list_index(l, &non_existent));
      list_destroy(&l);
    IT_END

  DESCRIBE_END

  DESCRIBE("list_get(list*, element*, index)")

    IT("should be able to get an element")
      int elements[] = {4, 3};
      int result = 7;
      list_t* l = list_create(sizeof(int));
      list_insert(l, elements, 0);
      list_insert(l, elements + 1, 1);
      list_get(l, &result, 1);
      SHOULD_EQ(3, result);
      list_destroy(&l);
    IT_END

    IT("should return an error when passed an invalid positive index");
      int element = 5;
      list_t* l = list_create(sizeof(int));
      SHOULD_EQ(-1, list_get(l, &element, 1));
      list_destroy(&l);
    IT_END;

    IT("should return an error when passed a negative index")
      int element = 5;
      list_t* l = list_create(sizeof(int));
      SHOULD_EQ(-1, list_get(l, &element, -1));
      list_destroy(&l);
    IT_END

  DESCRIBE_END

  DESCRIBE("list_rem(list*, element*, index)")

    IT("should be able to remove an element")
      int elements[] = {4, 3};
      int result;
      list_t* l = list_create(sizeof(int));
      list_insert(l, elements, 0);
      list_insert(l, elements + 1, 1);
      list_rem(l, &result, 1);
      SHOULD_EQ(3, result);
      SHOULD_EQ(1, list_size(l));
      list_destroy(&l);
    IT_END

    IT("should return an error when passed a negative index")
      int element = 5;
      list_t* l = list_create(sizeof(int));
      SHOULD_EQ(-1, list_rem(l, &element, -1));
      list_destroy(&l);
    IT_END

    IT("should return an error when passed an invalid positive index")
      int element = 5;
      list_t* l = list_create(sizeof(int));
      SHOULD_EQ(-1, list_rem(l, &element, 1));
      list_destroy(&l);
    IT_END

  DESCRIBE_END
}

void Testsuite_List(void)
{
  Specifications();
  UCSPEC_WriteSummary();
}

int main(void)
{
    UCUNIT_Init();
    UCSPEC_UseStaticsAtLeastOnce();
    UCUNIT_WriteString("\n**************************************\n\n");
    Testsuite_List();
    UCUNIT_Shutdown();

    return 0;
}
