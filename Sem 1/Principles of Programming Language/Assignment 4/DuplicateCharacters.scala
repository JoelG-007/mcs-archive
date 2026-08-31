import scala.io.StdIn
object DuplicateCharacters{
  def main(args: Array[String]): Unit ={
    print("Enter a string: ")
    val str = StdIn.readLine()
    println("Duplicate Characters:")

    for(i <- 0 until str.length){
      var count = 0
      var alreadyPrinted = false

      // Check if the character appeared before
      for(j <- 0 until i){
        if(str.charAt(i) == str.charAt(j))
          alreadyPrinted = true
      }

      // Count occurrences
      if(!alreadyPrinted){

        for(k <- 0 until str.length){
          if(str.charAt(i) == str.charAt(k))
            count += 1
        }

        if(count > 1)
          println(str.charAt(i) + " -> " + count)
      }
    }
  }
}