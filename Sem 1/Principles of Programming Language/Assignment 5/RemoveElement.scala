import scala.io.StdIn
object RemoveElement{
  def main(args: Array[String]): Unit ={
    val list = List(10, 20, 30, 40, 50, 60)

    println("Original List: " + list)

    // Remove by Value
    print("Enter value to remove: ")
    val value = StdIn.readInt()

    var newList = List[Int]()

    for(num <- list){
      if(num != value)
        newList = newList :+ num
    }

    println("After Removing Value: " + newList)

    // Remove by Index
    print("Enter index to remove: ")
    val index = StdIn.readInt()

    if (index >= 0 && index < newList.length){
      var finalList = List[Int]()

      for(i <- 0 until newList.length){
        if(i != index)
          finalList = finalList :+ newList(i)
      }

      println("After Removing Index: " + finalList)
    }else{
      println("Invalid Index")
    }
  }
}