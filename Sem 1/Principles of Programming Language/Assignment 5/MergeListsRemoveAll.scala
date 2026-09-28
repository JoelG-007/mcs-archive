// Removing all duplicates using count
import scala.io.StdIn
object MergeListsRemoveAll{
  def main(args: Array[String]): Unit = {
    val list1 = List(1, 2, 3, 4)
    val list2 = List(3, 4, 5, 6)

    println("List 1: " + list1)
    println("List 2: " + list2)

    print("Enter a new element: ")
    val element = StdIn.readInt()

    val merged = list1 ++ list2 :+ element

    var result = List[Int]()
    var i = 0

    while(i < merged.length){
      var count = 0
      var j = 0
      while(j < merged.length){
        if(merged(i) == merged(j)){
          count += 1
        }
        j += 1
      } 
      if(count == 1){
        result = result :+ merged(i)
      }

      i += 1
    }
    println("Final List: " + result)
  }
}
